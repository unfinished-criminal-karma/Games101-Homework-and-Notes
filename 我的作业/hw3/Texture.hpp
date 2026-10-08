//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        // 框架原版没有边界保护：spot 模型的 u 最小到 -0.052，直接 u*width 会得到负的列索引，
        // Debug 下 cv::Mat::at 断言失败、Release 下越界读。先把 uv 夹到 [0,1]，
        // 再乘 (width-1)/(height-1)，保证索引永远落在 [0, size-1]。
        if (u < 0.0f) u = 0.0f; else if (u > 1.0f) u = 1.0f;
        if (v < 0.0f) v = 0.0f; else if (v > 1.0f) v = 1.0f;
        auto u_img = u * (width - 1);
        auto v_img = (1 - v) * (height - 1);
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }
    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        if (u < 0.0f) u = 0.0f; else if (u > 1.0f) u = 1.0f;
        if (v < 0.0f) v = 0.0f; else if (v > 1.0f) v = 1.0f;
        auto u_img = u * (width - 1);
        auto v_img = (1 - v) * (height - 1);
        auto minu = floor(u_img);
        auto minv = floor(v_img);
        auto maxu = ceil(u_img);
        auto maxv = ceil(v_img);
        auto s = u_img - minu;
        auto t = v_img - minv;
        auto color00 = image_data.at<cv::Vec3b>(minv, minu);//先V后U
        auto color01 = image_data.at<cv::Vec3b>(maxv, minu);
        auto color10 = image_data.at<cv::Vec3b>(minv, maxu);
        auto color11 = image_data.at<cv::Vec3b>(maxv, maxu);
        Eigen::Vector3f u00(color00[0], color00[1], color00[2]);
        Eigen::Vector3f u10(color10[0], color10[1], color10[2]);
        Eigen::Vector3f u01(color01[0], color01[1], color01[2]);
        Eigen::Vector3f u11(color11[0], color11[1], color11[2]);
        Eigen::Vector3f u0 = u00 + (u10 - u00) * s;
        Eigen::Vector3f u1 = u01 + (u11 - u01) * s;
        return u0 + (u1 - u0) * t;
    }
};
#endif //RASTERIZER_TEXTURE_H
