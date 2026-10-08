// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)//已将int 改为float//
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vector3f point;
    point<<x,y,1;
    bool inside = false;
    float c1 = ((_v[0]-_v[1]).cross(_v[0]-point)).z(); 
    float c2 = ((_v[1]-_v[2]).cross(_v[1]-point)).z(); 
    float c3 = ((_v[2]-_v[0]).cross(_v[2]-point)).z();
    if ((c1 > 1e-6 && c2 > 1e-6 && c3 > 1e-6) || (c1 < -1e-6 && c2 < -1e-6 && c3 < -1e-6))
    {
       inside =  true;
    }
        return inside;
    
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        //rasterize_triangle(t);
        rasterize_triangle_MSAA(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t)
{
    auto v = t.toVector4();
    
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    int minx =(int) floor(std::min({v[0].x(),v[1].x(),v[2].x()}));
    int miny =(int) floor(std::min({v[0].y(),v[1].y(),v[2].y()}));
    int maxx =(int) ceil(std::max({v[0].x(),v[1].x(),v[2].x()}));
    int maxy =(int) ceil(std::max({v[0].y(),v[1].y(),v[2].y()}));
    
    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;
    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
    for (int x = minx; x < maxx; ++x)
    {
        for (int y = miny; y < maxy; ++y)
        {
            if (insideTriangle(x+0.5,y+0.5,t.v))
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(x+ 0.5, y+ 0.5, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;
                int index = get_index(x, y);
                if (depth_buf[index] > z_interpolated)
                {
                    depth_buf[index] = z_interpolated;
                    set_pixel(Eigen::Vector3f(x,y,z_interpolated),t.getColor());
                }
            }
        }
    }
}
void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
    std::fill(sample_depth_buf.begin(), sample_depth_buf.end(), std::numeric_limits<float>::infinity());//清除MSAA的深度缓存//
    std::fill(sample_color_buf.begin(), sample_color_buf.end(), Eigen::Vector3f{0, 0, 0});//清除MSAA的颜色缓存//
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    sample_depth_buf.resize(w * h * 4);//深度缓存
    sample_color_buf.resize(w * h * 4);//颜色缓存
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;//获取x+y值设为序号//
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}
int rst::rasterizer:: sample_index(int x, int y,int i,int j)
{
    if (i == 1 && j == 1)return get_index(x,y) * 4;
    else if (i == 1 && j == 3)return get_index(x,y) * 4+1;
    else if (i == 3 && j == 1)return get_index(x,y) * 4+2;
    else if (i == 3 && j == 3)return get_index(x,y) * 4+3;
    else return 0;
}
void rst::rasterizer::rasterize_triangle_MSAA(const Triangle& t)
{
    auto v = t.toVector4();
    int minx =(int) floor(std::min({v[0].x(),v[1].x(),v[2].x()}));
    int miny =(int) floor(std::min({v[0].y(),v[1].y(),v[2].y()}));
    int maxx =(int) ceil(std::max({v[0].x(),v[1].x(),v[2].x()}));
    int maxy =(int) ceil(std::max({v[0].y(),v[1].y(),v[2].y()}));
    for (int x = minx; x < maxx; x++)
    {
        for (int y = miny; y < maxy; y++)
        {
            for (int i = 1;i <= 3;i+=2)
            {
                for (int j = 1;j <= 3;j+=2)
                {
                    float realx = x+ i * 0.25;
                    float realy = y+j * 0.25;
                    if (!insideTriangle(realx, realy, t.v))continue;
                    auto [alpha1, beta1, gamma1] = computeBarycentric2D(realx, realy, t.v);//这部分用于重心坐标求值，Alpha，beta,gamma可以随便命名，但建议还是取这三个名字，这里跟上面重名了加了1//
                    float w_reciprocal = 1.0f / (alpha1 / v[0].w() + beta1 / v[1].w() + gamma1 / v[2].w());//详细部分看Lecture9
                    float z_interpolated = (alpha1 * v[0].z() / v[0].w() + beta1  * v[1].z() / v[1].w() + gamma1 * v[2].z() / v[2].w()) * w_reciprocal;
                    int si = sample_index(x, y, i , j);
                    if (z_interpolated < sample_depth_buf[si])
                    {
                        sample_depth_buf[si] = z_interpolated;
                        sample_color_buf[si] = t.getColor();
                        
                    }
                }
                
            }
            Eigen::Vector3f sum = Eigen::Vector3f::Zero();
            for (int k = 0; k < 4; k++) sum += sample_color_buf[get_index(x,y)*4 + k];
            set_pixel(Vector3f(x, y, 0), sum / 4.0f);//已经转换到视口空间，实际已经不需要z_Buffer的值//
        }
    }
}
// clang-format on