#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FPS = 60;
const int TIMESTEP = 1/FPS;


std::vector<std::vector<int>> GeneratePascalLookUpTable(int r, int s, int n){ //
    std::vector<std::vector<int>> pascalLookUpTable;

    // push initial base case
    std::vector<int> currentRow;
    currentRow.push_back(1);
    pascalLookUpTable.push_back(currentRow);
    currentRow.clear();

    // manually creating the lookup table
    for (size_t i = 0; i < r; i++)
    {
        for (size_t j = 0; j < pascalLookUpTable.back().size()+1; j++)
        {
            if(j == 0){
                currentRow.push_back(1);
                continue;
            }
            if(j >= pascalLookUpTable.back().size()){
                currentRow.push_back(1);
                continue;
            }
            if(j-1 >= 0 && j < pascalLookUpTable.back().size()){
                currentRow.push_back(pascalLookUpTable.back()[j-1] + pascalLookUpTable.back()[j]);
            }
        }
        pascalLookUpTable.push_back(currentRow);
        currentRow.clear();
    }

    return pascalLookUpTable;
}

std::vector<Vector2> ComputeForBezierCurve(int r, int s, int n, std::vector<int> lastPascalRow, std::vector<Vector2> controlPoints){
    std::vector<Vector2> points;
    float stepSize = (float)1/ (float) s;
    float currentStep = 0;
    // std::cout << "s: " << 1/s << std::endl;
    for (size_t i = 1; i <= s; i++)
    {
        currentStep += stepSize;
        Vector2 point;
        point.x = 0;
        point.y = 0;
        
        for (size_t j = 0; j < r+1; j++)
        {
            float f = 1 - currentStep;
            float g = currentStep;
            //std::cout << "control points: " << controlPoints[j].x << " ," << controlPoints[j].y << std::endl;
            point.x += lastPascalRow[j] * controlPoints[j].x * pow(f, r-j) * pow(g, j);
            point.y += lastPascalRow[j] * controlPoints[j].y * pow(f, r-j) * pow(g, j);
        }
        
        points.push_back(point);
    }
    return points;
    
}

std::vector<Vector2> GetTangents(int r, int v, int n, std::vector<int> secondToLastRow, std::vector<Vector2> controlPoints){
    std::vector<Vector2> tangents;
    float stepSize = (float)1/ (float) v;
    float currentStep = 0;

    for (size_t i = 1; i <= v; i++)
    {
        currentStep += stepSize;
        Vector2 point;
        point.x = 0;
        point.y = 0;
        
        for (size_t j = 0; j < r; j++)
        {
            float f = 1 - currentStep;
            float g = currentStep;
            
            //std::cout << "current step: " << currentStep << std::endl;
            point.x +=  (controlPoints[j+1].x - controlPoints[j].x) * r * secondToLastRow[j] * pow(f, r-j) * pow(g, j);
            point.y +=  (controlPoints[j+1].y - controlPoints[j].y) * r * secondToLastRow[j] * pow(f, r-j) * pow(g, j);
            /*std::cout << "control points for tangent calculation: (" << controlPoints[j+1].x << " ," << controlPoints[j+1].y << ") and (" 
            << controlPoints[j].x << " ," << controlPoints[j].y << std::endl; */
            
        }
        tangents.push_back(point); // Vector2Normalize(point)
        //std::cout << "Tangents: " << point.x << " ," << point.y << std::endl;
    }
    return tangents;
}

std::vector<Vector2> GetTangentNormals(std::vector<Vector2> tangents){
    std::vector<Vector2> tangentNormals;
    
    for (size_t i = 0; i < tangents.size(); i++)
    {
        tangentNormals.push_back(Vector2Normalize(tangents[i]));
    }
    return tangentNormals;
}

void draw(std::vector<Vector2> bezierCurvePoints, std::vector<Vector2> controlPoints, float radius, std::vector<Vector2> tangents, std::vector<Vector2> tangentNormals){
    for (size_t i = 0; i < controlPoints.size(); i++)
    {
        DrawCircle(controlPoints[i].x, controlPoints[i].y, radius, GREEN); 
    }
    for (size_t i = 0; i < tangents.size(); i++)
    {
        //DrawLine(bezierCurvePoints[tangents.size()*i].x, bezierCurvePoints[tangents.size()*i].y, tangents[i].x, tangents[i].y,RED);
        Vector2 temp = Vector2Rotate(tangentNormals[i], 1.5708);
        
        DrawLine(bezierCurvePoints[tangents.size()*i].x, 
                 bezierCurvePoints[tangents.size()*i].y,
                 bezierCurvePoints[tangents.size()*i].x + temp.x * 30, 
                 bezierCurvePoints[tangents.size()*i].y + temp.y * 30,RED);
    }
    for (size_t i = 0; i < bezierCurvePoints.size(); i++)
    {
        if(i+1 >= bezierCurvePoints.size()){
            continue;
        }
        DrawLine(bezierCurvePoints[i].x, bezierCurvePoints[i].y, bezierCurvePoints[i+1].x, bezierCurvePoints[i+1].y, YELLOW);
    }
}

bool IsControlPointValid(int r, int n, int x){ // work in progress
    return ( ( (r + 1) + (r * x) ) == r);
}

int main(){
    int r; // order of the curve
    int s; // steps
    int v;
    int n;
    float radius = 5;
    std::cout << "Input Order of the Curve:" << std::endl;
    std::cin >> r;
    std::cout << "Input steps:" << std::endl;
    std::cin >> s;
    std::cout << "Input number of times Tangents will be drawn:" << std::endl;
    std::cin >> v;
    std::cout << "Input number of control points:" << std::endl;
    std::cin >> n; 
    if(!IsControlPointValid(r, n, 0)){
        std::cout << "Is Control Point Valid " << ( ( (r + 1) + (r * 0) ) == n) <<std::endl;
    }
    // make sure to check if this is valid at this step (check slides)
    std::vector<Vector2> controlPoints;
    for (size_t i = 0; i < n; i++)
    {
        Vector2 vector;
        std::cin >> vector.x;
        std::cin >> vector.y;
        controlPoints.push_back(vector);
    }

    std::vector<std::vector<int>> lookupTable = GeneratePascalLookUpTable(r, s, n);
    std::vector<int> lastRow = lookupTable[lookupTable.size()-1];
    std::vector<Vector2> bezierCurvePoints = ComputeForBezierCurve(r, s, n, lastRow, controlPoints);


    std::vector<int> secondToLastRow = lookupTable[lookupTable.size()-2];    
    std::vector<Vector2> tangents = GetTangents(r, v, n, secondToLastRow, controlPoints);
    std::vector<Vector2> tangentNormals = GetTangentNormals(tangents);

    /*/ debug /
    for (size_t i = 0; i < secondToLastRow.size(); i++)
    {
        std::cout << secondToLastRow[i] << " ";
    }
    for (size_t i = 0; i < bezierCurvePoints.size(); i++)
    {
        std::cout << "Point " << i << " : " << bezierCurvePoints[i].x << ", " << bezierCurvePoints[i].y << std::endl;
    }
    for (size_t i = 0; i < lookupTable.size(); i++)
    {
        for (size_t j = 0; j < lookupTable[i].size(); j++)
        {
            std::cout << lookupTable[i][j] << " ";
        }
        std::cout << std::endl;
    }*/

    InitWindow(800, 600, "Tamano - Exercise2");

    while(!WindowShouldClose()){
        
        if(IsMouseButtonDown(0)){
            for (size_t i = 0; i < controlPoints.size(); i++)
            {
                if(CheckCollisionPointCircle(GetMousePosition(), controlPoints[i], radius+5)){
                    controlPoints[i] = GetMousePosition();
                    bezierCurvePoints = ComputeForBezierCurve(r, s, n, lastRow, controlPoints); // compute only when moving the curve
                    tangents = GetTangents(r, v, n, secondToLastRow, controlPoints);
                    tangentNormals = GetTangentNormals(tangents);
                }
            }
        }
        BeginDrawing();

        ClearBackground(BLACK);
        draw(bezierCurvePoints, controlPoints, radius, tangents, tangentNormals);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}