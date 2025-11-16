#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <vector>
#include <string>

struct Circle {
    double center_x, center_y, radius;
};
bool isPointInCircle(double x, double y, const Circle& circle) {
    double dx = x - circle.center_x;
    double dy = y - circle.center_y;
    return dx * dx + dy * dy <= circle.radius * circle.radius;
}
bool isPointInIntersection(double x, double y, const std::vector<Circle>& circles) {
    for (const auto& circle : circles) {
        if (!isPointInCircle(x, y, circle)) {
            return false;
        }
    }
    return true;
}
double calculateExactArea() {
    return 0.25 * M_PI + 1.25 * asin(0.8) - 1.0;
}
double monteCarloArea(
    const std::vector<Circle>& circles,
    double x_min, double x_max, 
    double y_min, double y_max,
    int num_points
) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_x(x_min, x_max);
    std::uniform_real_distribution<> dist_y(y_min, y_max);
    int points_inside = 0;
    for (int i = 0; i < num_points; ++i) {
        double x = dist_x(gen);
        double y = dist_y(gen);
        
        if (isPointInIntersection(x, y, circles)) {
            points_inside++;
        }
    }
    double area_rect = (x_max - x_min) * (y_max - y_min);
    return (static_cast<double>(points_inside) / num_points) * area_rect;
}
void runExperiments(const std::vector<Circle>& circles) {
    std::ofstream wide_file("wide_area_results.csv");
    std::ofstream narrow_file("narrow_area_results.csv");
    wide_file << "N,ApproximateArea,RelativeError\n";
    narrow_file << "N,ApproximateArea,RelativeError\n";
    double exact_area = calculateExactArea();
    std::cout << "Точная площадь: " << exact_area << std::endl;
    double wide_x_min = 0.5, wide_x_max = 2.5;
    double wide_y_min = 0.5, wide_y_max = 2.5;
    double wide_area = (wide_x_max - wide_x_min) * (wide_y_max - wide_y_min);
    double narrow_x_min = 1.0, narrow_x_max = 2.0;
    double narrow_y_min = 1.0, narrow_y_max = 2.0;
    double narrow_area = (narrow_x_max - narrow_x_min) * (narrow_y_max - narrow_y_min);
    for (int N = 100; N <= 100000; N += 500) {
        double wide_approx = monteCarloArea(circles, 
            wide_x_min, wide_x_max, wide_y_min, wide_y_max, N);
        double wide_error = abs(wide_approx - exact_area) / exact_area;
        wide_file << N << "," << wide_approx << "," << wide_error << "\n";
        double narrow_approx = monteCarloArea(circles,
            narrow_x_min, narrow_x_max, narrow_y_min, narrow_y_max, N);
        double narrow_error = abs(narrow_approx - exact_area) / exact_area;
        narrow_file << N << "," << narrow_approx << "," << narrow_error << "\n";
        
        if (N % 10000 == 0) {
            std::cout << "N = " << N << ": Широкая = " << wide_approx 
                 << ", Узкая = " << narrow_approx << std::endl;
        }
    }
    wide_file.close();
    narrow_file.close();
    std::cout << "Результаты сохранены в wide_area_results.csv и narrow_area_results.csv" << std::endl;
}

int main() {
    std::vector<Circle> circles = {
        {1.0, 1.0, 1.0},
        {1.5, 2.0, sqrt(5.0) / 2.0},
        {2.0, 1.5, sqrt(5.0) / 2.0}
    };    
    runExperiments(circles);
    return 0;
}