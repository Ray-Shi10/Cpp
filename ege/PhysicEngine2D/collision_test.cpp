#include <iostream>
#include "phyEngine2D.h"

int main() {
    Rect rect1(vec(0, 0), vec(10, 10));
    Circle circle1(vec(11.8, 9.99), 2);
    CollInfo info1 = rect1.intersect(circle1);
    std::cout << rect1 << std::endl;
    std::cout << circle1 << std::endl;
    std::cout << info1 << std::endl;
    
    // // 测试用例2: 圆形在矩形边缘
    // std::cout << "=== 测试2: 圆形在矩形右边缘 ===" << std::endl;
    // Rect rect2(vec(0, 0), vec(6, 3));  
    // Circle circle2(vec(7, 2), 2);        // 圆心在(7,0)，应该与右边缘相交
    // CollInfo info2 = rect2.intersect(circle2);
    // std::cout << "矩形: " << rect2 << std::endl;
    // std::cout << "圆形: " << circle2 << std::endl;
    // std::cout << "碰撞信息: " << info2 << std::endl;
    // std::cout << "是否碰撞: " << (info2 ? "是" : "否") << std::endl << std::endl;
    
    // // 测试用例3: 圆形在矩形角落
    // std::cout << "=== 测试3: 圆形在矩形右上角 ===" << std::endl;
    // Rect rect3(vec(0, 0), vec(6, 8));  
    // Circle circle3(vec(7, 7), 2);        // 圆心在(7,7)，应该与右上角相交
    // CollInfo info3 = rect3.intersect(circle3);
    // std::cout << "矩形: " << rect3 << std::endl;
    // std::cout << "圆形: " << circle3 << std::endl;
    // std::cout << "碰撞信息: " << info3 << std::endl;
    // std::cout << "是否碰撞: " << (info3 ? "是" : "否") << std::endl << std::endl;
    
    // // 测试用例4: 圆形完全在矩形外部
    // std::cout << "=== 测试4: 圆形完全在矩形外部 ===" << std::endl;
    // Rect rect4(vec(0, 0), vec(10, 10));  
    // Circle circle4(vec(20, 20), 2);      // 圆心在(20,20)，应该不相交
    // CollInfo info4 = rect4.intersect(circle4);
    // std::cout << "矩形: " << rect4 << std::endl;
    // std::cout << "圆形: " << circle4 << std::endl;
    // std::cout << "碰撞信息: " << info4 << std::endl;
    // std::cout << "是否碰撞: " << (info4 ? "是" : "否") << std::endl << std::endl;
    
    // // 测试用例5: 边界情况 - 圆形刚好接触矩形边缘
    // std::cout << "=== 测试5: 圆形刚好接触矩形边缘 ===" << std::endl;
    // Rect rect5(vec(0, 0), vec(10, 10));  
    // Circle circle5(vec(7, 0), 3);        // 圆心在(7,0)，半径3，应该刚好接触
    // CollInfo info5 = rect5.intersect(circle5);
    // std::cout << "矩形: " << rect5 << std::endl;
    // std::cout << "圆形: " << circle5 << std::endl;
    // std::cout << "碰撞信息: " << info5 << std::endl;
    // std::cout << "是否碰撞: " << (info5 ? "是" : "否") << std::endl << std::endl;
    
    return 0;
}