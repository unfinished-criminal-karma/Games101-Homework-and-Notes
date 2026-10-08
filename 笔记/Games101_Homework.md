本部分为作业思路讲解，记录我学习Games101作业遇到的思路，问题以及解决方法，也希望能为你提供思路
感谢闫老师的课程

##~1BIHW1

第一次接触C++Eigen库，不了解库里函数用法，查一下资料
Eigen 是一个 header-only 的 C++ 模板线性代数库，主要用来做矩阵、向量、数组、线性方程求解、分解、几何变换和稀疏矩阵计算。
也就是大部分线性代数运算都能用库都包括，简单看一下用法
读一下要求：
 get_model_matrix(float rotation_angle): 逐个元素地构建模型变换矩阵并返回该矩阵。在此函数中，你只需要实现三维中绕z轴旋转的变换矩阵，而不用处理平移与缩放
get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar): 使用给定的参数逐个元素地构建透视投影矩阵并返回

继续观察代码，要处理变换需要用到矩阵乘法，但不会构造矩阵，
由Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)的内容推出构造方法
不了解渲染代码含义，简单分析函数结构和变量名称了解

##~2get model matrix函数：
要绕z轴旋转么也就是在XOY平面旋转，传入旋转角度，正好和课程Lecture4部分内容相同，直接用公式

//注意容易出错的一个点是传入角度是角度而不是弧度，sin/cos使用的是弧度，需要转换

##~2get_projection_matrix函数：
传入FOV,宽高比，近平面距离和远平面距离，
参数处理在Lecture5讲过，直接用
透视投影变换 = 正交投影矩阵 * 挤压矩阵（将视椎体挤压成立方体） //矩阵从右往左作用，先挤压再投影，将视椎体挤压成立方体再正交投影
//注意矩阵左右乘法得到结果不同
分别算，
正交投影矩阵：缩放公式和平移公式相乘（图1）
挤压矩阵，在Lecture4末尾部分（图2）
 
##~2附加题：
在 main.cpp 中构造一个函数，该函数的作用是得到绕任意过原点的轴的旋转变换矩阵。
Eigen::Matrix4f get_rotation(Vector3f axis, float angle)
使用Lecture3提到的罗德里格斯旋转公式，Eigen库里有

Eigen::AngleAxisf rot(realangle, n);求出绕n旋转realangle弧度的旋转矩阵
求出的旋转矩阵是三维方阵还需要进一步转换为四维方阵
model.topLeftCorner<3,3>() = rot.toRotationMatrix();   // 把 3×3 塞进 4×4 的左上角，返回的model是四维矩阵
得到函数Eigen::Matrix4f get_rotation(Vector3f axis, float angle)

##~1BIHW2
要求：
rasterize_triangle(): 执行三角形栅格化算法
 static bool insideTriangle(): 测试点是否在三角形内。你可以修改此函数的定义，这意味着，你可以按照自己的方式更新返回类型或函数参数。

分析文件格式，main.cpp和作业一相同（Projection translation函数需要从第一次作业复制）
要修改的函数在rasterizer.cpp中，rasterier.hpp是对应的头文件，
观察栅格化算法：传入const Triangle& t,不清楚的变量，查Triangle.hpp的定义，是一个三角形的类
观察点位置与三角形关系判断函数：输入一个点（x,y），Vector3f* _v //不清楚含义，去Triangle.hpp看定义，_v是三维变量数组代表三角形三顶点坐标

##~2insideTriangle
明显insideTriangle更容易实现，第三讲提过向量叉乘法判断点是否在三角形内    //同向叉积法
叉乘的时候发现二维和三维点无法相减求向量，用点的齐次化写法（x,y,0）//z不影响叉乘后z方向的正负号，只影响大小

##~2转到栅格化算法：
第一步：求Bounding Box，
观察得到Bounding Box的边界就是三角形上点的横纵坐标最大值最小值的组合
传入的Triangle类成员有三个顶点坐标信息//还是不用管z坐标，z只影响深度缓存判断遮挡关系

第二步：
注释里给出求缓存的Z-Buffer部分
对每个像素的中心位置进行判断是否在三角形内（见图3），发现inside_Triangle传入的是整数，题目说可以改传入参数类型，改成float
但要注意浮点数大小判断 c1 > 1e-6就可视为正数
如果在三角形内，需要获取Z-Buffer进行比较    //同时注意获取Z-Buffer部分也好换成中心位置
注意要求中：请注意我们是如何初始化depthbuffer 和注意zvalues 的符号。为了方便同学们写代码，我们将z进行了反转，保证都是正数，并且越大表示离视点越远。
Get_Index函数获取序号
Z-Buffers数组内对应序号的就是当前存储的深度值
根据变量名得出z_interpolated为计算出的深度值
保留较小值

第三步：
每个点确定深度值之后注释提示调用set_pixel函数设置点的深度颜色//set_pixel传入一个三维向量点坐标和三维向量颜色，z坐标用z_Buffer //实际上函数里的运算根本没有用z坐标


写完后运行：出问题，上下左右颠倒，其余正常（遮挡正确，形状完整）
等等，我有印象，翻课程，Lecture8提到因为可能是左右手系的问题，推导可能跟实际有出入，先不管

/*后面研究了一下，
view 矩阵	把相机摆到 +z、看向 −z → 导致前方物体的 z 是负的
投影矩阵的 w 行（[0,0,1,0]）	让 w = z，于是透视除法除以负数
透视除法 vec /= vec.w()	翻转
视角变换让 z 变成了负数，投影又拿这个负数当除数

解决方法 ：把投影矩阵的 w 行改成 [0,0,-1,0]（用 −z，也就是"到相机的距离"，正数）
并且直接用
               [ 1/(aspect·tan(fov/2))            0              0              0                ]
M_proj =[          0                         1/tan(fov/2)      0              0                ]
               [          0                                 0        -(f+n)/(f-n)   -2fn/(f-n)     ]
               [          0                                 0             -1              0                ]作为投影矩阵 
这也是 OpenGL 标准矩阵的做法
当w为-1时，这个标准矩阵在数学上直接相当于课程中的正交投影矩阵和挤压矩阵的外积*/

##~2附加题：进行多重采样计算MSAA
MSAA在每个像素用多个采样点，采样完成后根据信息去掉高频信息后采样，根据像素内在三角形内采样点数量来模糊颜色    //后面发现有黑边问题
要实现MSAA无疑要对文件结构以及函数作用了解更深，涉及到修改序号（之前一个像素对应一个序号，现在一个像素对应四个序号），深度缓存（如何将不同序号对应不同Z_Buffer）
在头文件内添加新的z_Buffer，函数
std::vector<float> sample_depth_buf;//构造四倍深度缓存，用原深度缓存越界会崩溃//
void rasterize_triangle_MSAA(const Triangle& t)；
把depth——buf有的也给 sample_depth_buf做一遍
在构造函数创建
sample_depth_buf.resize(w * h * 4);//深度缓存

在clear函数清理
std::fill(sample_depth_buf.begin(), sample_depth_buf.end(), std::numeric_limits<float>::infinity());//清除MSAA的深度缓存//

创建函数定义，,仍然先循环每一个像素，每个像素点取四个采样点，我用的是i,j循环数字1,3分别乘0.25即可得分别得到x+0.25,x+0.75,y+0.25,y+0.75的组合//也可以用偏移表
const float offs[4][2] = {{0.25f,0.25f}, {0.75f,0.25f},
                                     {0.25f,0.75f}, {0.75f,0.75f}};
for (int s = 0; s < 4; ++s) {
    float sx = x + offs[s][0];
    float sy = y + offs[s][1];
    int   si = get_index(x, y) * 4 + s;     

使用sample_depth_Buf的时候调用新函数int sample_index(int x, int y,int i,int j);获取新序号
设置inside_numt统计每个像素在三角形内的采样点数，set_pixel的时候乘以(inside_num/4.0f) //最终方案不需要

运行，A,B三角形交界处出现黑边，见图4

检查
交界处sample_depth_Buffer较小的三角形对应像素正常，另一个为黑色
边界像素：A（近，绿）覆盖左边 2 个采样点
          B（远，蓝）覆盖右边 2 个采样点
A先存储颜色B后存储将A颜色覆盖
 边界像素最终颜色：ColorB × 0.5 + 黑 × 0.5
每个像素点颜色也要缓存，不需要统计在三角形内采样点数，直接取四个采样点平均
加std::vector<Vector3f> sample_color_buf;//构造四倍颜色缓存//
同样在构造函数，clear函数中创建，清理

启动，正常，抗锯齿效果明显，作业二圆满完成

##~1BIHW3
本次作业七个要求：
| 1 | 修改函数 `rasterize_triangle(const Triangle& t)` in `rasterizer.cpp`:在此处实现与作业 2 类似的插值算法,实现**法向量、颜色、纹理颜色**的插值 
| 2 | 修改函数 `get_projection_matrix()` in `main.cpp`:将你自己在之前的实验中实现的投影矩阵填到此处 
| 3 | 修改函数 `phong_fragment_shader()` in `main.cpp`:实现 **Blinn-Phong 模型**计算 Fragment Color 
| 4 | 修改函数 `texture_fragment_shader()` in `main.cpp`:**在实现 Blinn-Phong 的基础上**,将**纹理颜色视为公式中的 kd**,实现 Texture Shading 
| 5 | 修改函数 `bump_fragment_shader()` in `main.cpp`:**在实现 Blinn-Phong 的基础上**,仔细阅读该函数中的注释,实现 **Bump mapping** 
| 6 | 修改函数 `displacement_fragment_shader()` in `main.cpp`:**在实现 Bump mapping 的基础上**,实现 **displacement mapping**
附加题：
| 7 | **双线性纹理插值**:使用双线性插值进行纹理采样,在 `Texture` 类中实现一个新方法 `Vector3f getColorBilinear(float u, float v)` 
并通过 fragment shader 调用它。为了使效果更明显,应考虑选择**更小的纹理图**。请同时提交纹理插值与双线性纹理插值的结果并比较

大致看一眼内容结构和上次作业相似
多了几个函数
##~2分析要求1函数
参数：传入三角形类，三个三维向量视口坐标
观察作业二rasterize_triangle函数
TODO解释了下面注释参数含义

TODO: Inside your rasterization loop:                       
//    * v[i].w() is the vertex view space depth value z.        
//    * Z is interpolated view space depth for the current pixel 
//    * zp is depth between zNear and zFar, used for z-buffer     

// float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());    
 // float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
// zp *= Z;

每个像素循环中
v[i].w() 是顶点视口深度z值 
Z是插值后当前像素视口深度
zp是zNear和zFar距离,用于z-fuffer

分析注释，这是重心坐标插值部分，如果点在三角形内一定存在α+β+γ = 1且都大于0（在Lecture9部分）//中心坐标见图5图6
有interpolate函数可以用来算重心坐标插值
还是用depth_buf存储深度
/*这里读完就可以分析computeBarycentric2D是求三角形内一个点的α β γ，*/
作业二框架可以直接套用  //insideTriangle函数参数记得改成float，或者用α β γ同时>=0用重心坐标判断

// Use: fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
// Use: payload.view_pos = interpolated_shadingcoords;
// Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
// Use: auto pixel_color = fragment_shader(payload);

提示插值出的参数用于片元着色//第四个参数明显是三元运算符，没有自动传空指针
提示插值后interpolated_shadingcoords用法
提示三角形颜色不直接写入缓冲，将颜色传给着色器获取最终颜色//三角形类也没有get_color函数了
像素颜色 =  fragment_shader(payload);
查一下fragment_shader_payload定义,在shader.hpp里
是一个结构体，里面含有法线，颜色，纹理等参数
也就是说把插值出来的结果创建一个payload结构体来调
最后按注释提示顺序创建结构体，赋值，传参给 fragment_shader（）算颜色，最后set_pixel（）设置颜色
//需要注意的是set_pixel接受的是Vector2i但是传Vector2f不会报错，第一次顺手传了个Vector2f编译失败//

##~2分析要求2函数：
复制作业二的get_projection_matrix()函数

##~2分析要求3函数：
传入fragment_shader_payload也就是之前插值出的法线纹理等变量
看一眼定义的变量，ka，kd ，ks，和课程Lecture8内容符合//phong模型在图7
//注意⟪K_{a}⟫⟪I_{a}⟫和n · l两种乘法不同，一个是逐分量相乘，一个是向量点乘
存在两个light遍历计算
l,v,h都是方向向量需要归一化

/*需要注意环境光只要计算一次不用循环，而漫反射和高光需要
squaredNorm() 计算长度平方
cwiseProduct 向量逐分量相乘*/

##~2分析要求4函数：
在实现 Blinn-Phong 的基础上,将纹理颜色视为公式中的 kd
还是传入fragment_shader_payload

第一个TODO用纹理颜色替换原本的kd部分
Eigen::Vector3f kd = texture_color / 255.f;
texture_color << return_color.x(), return_color.y(), return_color.z();
直接从payload的Texture中取，uv也在payload里

第二个TODO和Phong模型一样
直接搬

##~2分析要求5函数：
还是传入fragment_shader_payload
第一个TODO注释给了伪代码
翻译成C++
向量定义，矩阵定义都做过

/*官方论坛助教提醒：
bump mapping 部分的 h(u,v) = texture_color(u,v).norm,u,v 是 tex_coords,w,h 是纹理的宽高
bump 与 displacement 中修改后的 normal 仍需 normalize
实现 h(u+1/w, v) 时必须写成 h(u+1.0/w, v) —— 整数除法陷阱
凹凸纹理正规做法是单通道灰度图;本课程为框架简便用了一张 RGB 图当凹凸贴图,需要指定规则把彩色投影到灰度
 —— 助教「恰好」选了 norm,为保证结果一致,统一用 norm*/
norm用于求向量长度
h(u,v)相当于求贴图中该点的值，调用texture的getColor函数

##~2分析要求6函数
依旧传入fragment_shader_payload
第一个TODO里注释和Bump.map基本一样
只多了point的偏移
第二个TODO
直接复制phong模型部分

##~2分析附加题要求7：
课程的双线性插值见图8
先定义函数，在 Texture 类中Vector3f getColorBilinear(float u, float v){}
先做边界保护，原版getColor代码没有边界保护运行的时候崩过一次//spot 模型的 u 最小到 -0.052，直接 u*width 会得到负的列索引，
在Lecture9 讲过，研究如何处理参数
尝试函数内定义Lerp函数,报错//C++不允许使用嵌套函数定义
手动实现
//注意顺序先v后u
不能Eigen::Vector3f color00 = image_data.at<cv::Vec3b>（minv, minu）；
cv::Vec3b 不能直接自动变成 Eigen::Vector3f
仿照getColor模式两步写//注意参数要先V后U
但写完要调用，而且要求建议换小一点的贴图，让AI生成小贴图
再写一个Eigen::Vector3f texture_bilinear_fragment_shader(const fragment_shader_payload& payload)调用双线性插值
复制texture_fragment_shader内容把里面getColor换成getColorBilinear
现在可以调用双线性插值了

如何加新贴图？
不会，问AI，在main函数里加这两条命令行分支//作业使用命令行调用不同渲染模型，也就是调用不同函数
else if (argc == 3 && std::string(argv[2]) == "texture_small")   // 最近邻 + 小图 → 马赛克
{
    active_shader = texture_fragment_shader;
    r.set_texture(Texture(obj_path + "spot_texture_64.png"));
}
else if (argc == 3 && std::string(argv[2]) == "bilinear")        // 双线性 + 小图 → 平滑
{
    active_shader = texture_bilinear_fragment_shader;
    r.set_texture(Texture(obj_path + "spot_texture_64.png"));
}

命令行语法：
Rasterizer.exe  <输出文件名>  <shader名>
                         argv[1]      argv[2]

shader名 可选：normal / texture / phong / bump / displacement / texture_small / bilinear
工作目录必须是 build —— 代码里模型用的是相对路径 ../models/spot/...
第一个参数必须带图片扩展名（.png / .jpg），否则 OpenCV 会报错
例：//第一行是文件地址
cd F:\Practice\GAMES101-hw3\build
.\Rasterizer.exe output_normal.png normal
.\Rasterizer.exe output_phong.png phong


运行不输入默认打开交互模式
如果求方便不想研究就直接改交互模式默认函数和默认纹理//在main函数中
默认函数std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = bump_fragment_shader;
默认纹理：texture_path = "hmap.jpg";
r.set_texture(Texture(obj_path + texture_path));
换 phong / normal：直接改，纹理用不着（不吃贴图）
换 bump / displacement：还得把 L385 那行改成 hmap.jpg（要高度图），否则跑出来是拿彩色贴图当高度，效果不对
换 bilinear：默认纹理已经是 spot_texture.png，但想要明显效果得配上小图（spot_texture_64.png）


命令行输出双线性插值测试：AI检查不够模糊//正常情况：64×64 的图被放大 4.6 倍，双线性把每个纹素晕开 4~5 个像素
原因：我直接用auto color00 = image_data.at<cv::Vec3b>(minv, minu)；进行插值，
但Vec3b 的减法是饱和减法，负数会自动变成0，精度丢失，中间值换成Vector3f
成功
HW3完成

##~1BIHW4
要求：
 bezier：该函数实现绘制Bézier 曲线的功能。它使用一个控制点序列和一个
OpenCV：：Mat 对象作为输入，没有返回值。它会使 t在0到1的范围内进
行迭代，并在每次迭代中使 t增加一个微小值。对于每个需要计算的 t，将
调用另一个函数 recursive_bezier，然后该函数将返回在 Bézier 曲线上 t
处的点。最后，将返回的点绘制在 OpenCV ：：Mat对象上
recursive_bezier：该函数使用一个控制点序列和一个浮点数 t 作为输入，
实现de Casteljau 算法来返回 Bézier 曲线上对应点的坐标

很明显第一个函数需要第二个函数的参与，先写第二个函数
贝塞尔曲线是Lecture11的内容（图9），闫老师从几何和代数都讲了其中内容，最后代数部分是伯恩斯坦多项式

##~2分析函数二：
传入控制点序列，先考虑实现四个点函数，将四个点翻译过来一步一步硬算（步骤和产生的点都是固定的）
 用 t: (1−t) 的比例细分每个线段，并找到该分割点，求线段的分点用A * (1-t) + B * t   //A,B为端点，鸡爪定理（高中知识）

/*多个点的实现：
改成递归函数，用一个temp向量数组存储下一代序列，再将下一代序列传入并调用该函数，size为0时停止
*/
##~2分析函数一：
传入序列和Window参数（应该是窗口类型，cv::Mat 不熟先不动）
分析发现，实质是将曲线离散化，每个t对应一个点，t每次增加微小值，由注释得t∈[0，1]
for循环遍历，每次+0.001，1000个点保证点足够密，
分析原有naive_bezier发现window.at<cv::Vec3b>(point.y, point.x)[2] = 255；控制颜色
要求化成绿色，分析如何改变颜色
分析color有关的代码，只在main函数的cv::cvtColor函数里出现
看不懂这个cv::COLOR_BGR2RGB，查AI：

cv::COLOR_BGR2RGB 是 OpenCV 中的一个颜色通道转换代码，通常配合 cv::cvtColor 使用。
把图像从 BGR 通道顺序 转换成 RGB 通道顺序。

也就是说Color是个三维向量分别表示BGR通道，结合原函数native_bazier函数画红色线，
尝试将(point.y, point.x)[2] = 255换为window.at<cv::Vec3b>(point.y, point.x)[1] = 255；
成功画出绿色线条且贝塞尔曲线正常

##~2附加题
实现对Bézier 曲线的反走样。(对于一个曲线上的点，不只把它对应于一个像
素，你需要根据到像素中心的距离来考虑与它相邻的像素的颜色。)

分析：需要检测周围的像素判断像素中心到点的位置，没要求如何衰减，直接用把距离平方映射到[0,1]//类似MSAA的模糊
用双层for循环auto temp_point =cv::Point2f(floor(point.x) + 0.5 + i,floor(point.y) + 0.5 + j);//i,j∈[-1,1]包括3×3的格子
判断distance的平方与0.5（⟪\sqrt{2}⟫/2的平方）和2（⟪\sqrt{2}⟫的平方）的大小关系，小于0.5为满亮，大于2为0，
中间部分按float ColorIntensity = 1.0 - distance2/2.0;求亮度，最后让Colorintensity与255相乘决定最终颜色
//注意边界检查，超过700（窗口大小700*700，也可以写window.cols（长）和window.ros（宽））直接continue
//算距离尝试用distance函数（UEC++写多了的后遗症），直接用古法硬算距离平方，后面才知道OpenCV有norm函数求距离
测试，出错，白点部分线条呈品红色且曲线不连续（我甚至换成了t += 0.0001）//见图10
调试
出现品红的原因是BGR三向量G为0，红色和蓝色混合成品红
白色是BGR三向量三部分均为255，应该是曲线的颜色把绿色写成0了
找到了
现在每个像素点颜色互相干扰，曲线的亮度就取决于"最后一个写它的是谁"，完全随机，画出来是断续的暗线且因为距离过远的点设置绿色成0，把白点附近的绿色设为0了
使用max函数取最大值，既解决了断续暗线有保证白点附近也正常，白点附近max就取255，其他地方也取最大值无暗线
uchar c = (uchar)(255 * ColorIntensity);//用uchar保证一致
auto &pixel = window.at<cv::Vec3b>(temp_point.y, temp_point.x);//auto只产生引用只改副本，加&
pixel[1] = std::max(pixel[1],c);

/*为什么只在白点附近品红？
其他点是黑色绿通道本来就为0*/


//思考题主要参考了知乎zhen大佬的文章
https://zhuanlan.zhihu.com/p/1888595594760656719?share_code=OG3vMdKTaAm0&utm_psn=2087917791584301286
HW4完成
##~1BIHW5
要求：
Renderer.cpp 中的 Render()：这里你需要为每个像素生成一条对应的光
线，然后调用函数 castRay()来得到颜色，最后将颜色存储在帧缓冲区的相
应像素中。
Triangle.hpp 中的 rayTriangleIntersect(): v0, v1, v2 是三角形的三个
顶点，orig 是光线的起点，dir 是光线单位化的方向向量。tnear, u, v 是你需
要使用我们课上推导的Moller-Trumbore 算法来更新的参数。
拿到的是一份新代码，刚拿到又回到了刚开始做HW1的感觉，新函数新参数太多看得头疼，
做前几份作业积累起的信心又没了，硬着头皮看，
只能通过函数名判断功能，代码太多，短时间只能梳理框架
大致理解，有问题遇到时再翻代码

直接看目标函数
函数二简单先挑软柿子
##~2分析函数二：
Moller-Trumbore算法，课上讲过（图11）
传入三角形三个点坐标，光的出发点，方向，t,u,v返回值为布尔
直接代公式//注意没用Eigen头文件，要用自定义的crossProduct和dotProduct
//注意要检查S · E是否等于0
如果t,u,v>0且u+v <= 1，则光线与三角形相交返回true
函数二结束

##~2分析函数一：
为每个像素生成一条对应的光线，跟课上讲的一样
看注释，找到当前的像素点的x,y找到方向向量
dir是方向向量z = -1，注释也提到要归一化
传入场景参数，包含窗口宽高，fov背景色等参数
相当于要求摄像机朝视口内每一个像素发出一道光，遍历每个像素点
画图理解（写笔记的时候我自己在UE也做了一下）//重点在于摄像机中心点过XOY平面原点
可以看出相当于在摄像机前1个单位处放置一个平面，分别求每个像素点的坐标//dirz = -1 因为摄像机朝向z负半轴
给出的代码i,j都从零开始，直接尝试遍历每个像素中心分别减去宽/高的一半（从左下角开始算），求得像素在视口的XY坐标
//后面发现其实要从左上角遍历，这个问题后面再说
不对，这样像素点x,y坐标太大跟z距离的1明显不对
现在研究视口的XY坐标如何转成观察空间正确的方向向量，
将坐标轴放到摄像机前一个单位,由图易得scale为视口半高，scale * imageAspectRatto为视口半宽
注释里提到要乘scale，
也就是前面要是占半高的比例，
//想半天原来是NDC，把长宽控制到[-1,1]
Y遍历后先除以半高再乘scale，归一化dir，尝试渲染/
输出ppm，windows图片处理器打不开，要用Photoshop，比例正常但上下颠倒（图13）
查了一下，ppm默认左上角开始存储，所以上下颠倒
改y部分为（height-j-0.5）/(height/2.0f) * scale
成了！（图14）

##~1BIHW6
这个作业是前面几个作业我做得最艰难的,思考题是真难写
直接看要求
首先，你需要从上一次编程练习中引用以下函数：
• Render() in Renderer.cpp: 将你的光线生成过程粘贴到此处，并且按照新框架更新相应调用的格式。
• Triangle::getIntersection in Triangle.hpp: 将你的光线-三角形相交函数粘贴到此处，并且按照新框架更新相应相交信息的格式。
在本次编程练习中，你需要实现以下函数：
• IntersectP(const Ray& ray, const Vector3f& invDir,const std::array<int, 3>& dirIsNeg) in the Bounds3.hpp: 这个函数的
作用是判断包围盒BoundingBox与光线是否相交，你需要按照课程介绍的算法实现求交过程。
• getIntersection(BVHBuildNode* node, const Ray ray)in BVH.cpp: 建立BVH之后，我们可以用它加速求交过程。该过程递归进行，你将在其中调
用你实现的Bounds3::IntersectP

先看第一部分
##~2Render函数
代码直接给定x,y，直接补最后归一化的方向向量和framebuffer
复制过来发现castRay报错，翻半天发现变成scene.hpp里的函数了
其他不用动

##~2 Triangle::getIntersection函数
传入参数变成了ray，仿照前面做法先设置t<0时返回inter
刚开始没看懂，注意PDF的代码框架的改动
 Intersection.hpp: 这个数据结构包含了相交相关的信息。
现在相交的参数要转到这边，也就是转到inter参数里
转到Intersection,hpp里看，要传happened，coords，normal，distance，obj，m六个参数
分别传是否相交，射线的交点（Ray.hpp重载了括号用于求向量末端），法线，三角形（这里用this关键字），材质（三角形已经指定了m）

第二部分
##~2IntersectP(const Ray& ray, const Vector3f& invDir,const std::array<int, 3>& dirIsNeg) 函数
分析注释和参数
ray是传入光线有关参数，invDir为光线xyz的倒数，dirIsNeg表示xyz方向是否为负
要求是否相交
和课上讲的类似,先看成直线
拆成xyz三个分量来算每个轴上对应的tmin和tmax
取三个tmin的最大值和tmax的最小值
//如果dirIsNeg为1则要交换该轴的tmin和tmax，因为方向反了pMin和Pmax距光源距离要交换
最后当且仅当tmin < tmax && tmax > 0返回true

##~2 getIntersection(BVHBuildNode* node, const Ray ray)函数
分析参数，
传入BVH节点和光线有关参数，返回一个intersection类参数
课程上提供了伪代码，直接对照翻译
先创建一个Intersection类参数
第一步判断是否相交，
先用节点是否为空判断，以防调用IntersectP时出事
再对节点的bound调用IntersectP
需要dirIsNeg参数，手动创建，根据光每个轴方向是否>0判断为0或1

第二步是否为叶子节点
判断是否有子节点，没有就返回object的getIntersction
有就hit1,2取较近点
完成，图片正常（图17）

##~2附加题
看到PPT了，知道公式了，你来实现它
我来实现它，真的假的？
不会翻一堆资料理解定义了还是不会写，和AI斗智300回合
决定使用精确扫描+排序的方法//也有用桶排序的方法我这里不提，因为不太会
磨半天磨出来了
BVH是一种加速结构：用逐层收紧的包围盒把几何组织成树。回答数据怎么组织
SAH是一种划分准则：构建 BVH 时判断这一刀切在哪最划算。它自己不产生任何结构。
当前写的是NAIVE法//BVH.hpp第28行写了两种方法名称
分析代码，
管理划分的函数是BVH.cpp的BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)函数
仿照定义一个BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*> objects)函数
直接修改BVH.cpp的BVHAccel函数
if (splitMethod == SplitMethod::SAH)
    {
        root = recursiveBuildSAH(primitives);
    }
    else
    {
        root = recursiveBuild(primitives);
    }
BVH.hpp第31行是划分准则，把它的NAIVE换成SAH
理论SAH思路：按最长轴的方向，按物品包围盒质心坐标从小到大依次枚举用公式算出期望，取期望最小的值
当且仅当包含图元数量小于阈值或最小期望大于不切时停止递归//这里只有一个图元所以
recursiveBuildSAH函数思路：这里取阈值为1，
公式需要父盒表面积，子盒表面积，左右包围盒包含的图元，访问一个图元开销，访问一个节点开销
//最后两个可以自己定义
先检查包围盒内是否只剩一个元素
遍历求全包围盒面积
求质心包围盒最大轴用来计算划分
NAIVE也是这样算，父盒最长轴反映的是"某个物体有多大"，
而划分要解决的是"物体之间有多分散”，用质心包围盒
不用担心切到图元，因为划分只决定每个图元在哪一边，包围盒的判断还是用BVH
求总图元个数，后续遍历每个物体质心位置计算每个右盒表面积存储备用
从前端遍历每个物体质心位置求左盒表面积
令最初Bestcost为无限，遍历算Cost取最小
最后把左右盒物体划分，分别用于递归左右子节点

最后成功运行，对比耗时
再main.cpp将倒数第二行代码换成毫秒
 std::cout << "          : " << std::chrono::duration<double, std::milli>(stop - start).count() << " ms\n";
NAIVE:4422.41ms 4287.15ms 3895ms
SAH:4007.79ms   3752.18ms  3441.44ms
优化了，额，好了一点

HW6结束

##~1BIHW7
阅读要求PDF:
相比上一次实验，本次实验对框架的修改较大，主要在以下几方面：
• 修改了main.cpp，以适应本次实验的测试模型CornellBox
• 修改了Render，以适应CornellBox 并且支持 Path Tracing 需要的同一 Pixel多次Sample
• 修改了Object，Sphere，Triangle，TriangleMesh，BVH，添加了 area 属性与
Sample 方法，以实现对光源按面积采样，并在Scene中添加了采样光源的接口sampleLight
• 修改了Material 并在其中实现了 sample, eval, pdf 三个方法用于 Path Tracing 变量的辅助计算

你需要从上一次编程练习中直接拷贝以下函数到对应位置：
• Triangle::getIntersection in Triangle.hpp: 将你的光线-三角形相交函数
粘贴到此处，请直接将上次实验中实现的内容粘贴在此。
• IntersectP(const Ray& ray, const Vector3f& invDir,
const std::array<int, 3>& dirIsNeg) in the Bounds3.hpp: 这个函数的
作用是判断包围盒BoundingBox与光线是否相交，请直接将上次实验中实现
的内容粘贴在此处，并且注意检查t_enter=t_exit的时候的判断是否正确。
• getIntersection(BVHBuildNode* node, const Ray ray)inBVH.cpp: BVH
查找过程，请直接将上次实验中实现的内容粘贴在此处

在本次实验中，你只需要修改这一个函数:
• castRay(const Ray ray, int depth)in Scene.cpp: 在其中实现 Path Tracing 算法

可能用到的函数有：
• intersect(const Ray ray)in Scene.cpp: 求一条光线与场景的交点
• sampleLight(Intersection pos, float pdf) in Scene.cpp: 在场景的所有
光源上按面积uniform 地 sample 一个点，并计算该 sample 的概率密度
•sample(constVector3fwi,const Vector3fN)inMaterial.cpp:按照该
材质的性质，给定入射方向与法向量，用某种分布采样一个出射方向
•pdf(constVector3fwi,const Vector3fwo,constVector3f N)inMaterial.cpp:
给定一对入射、出射方向与法向量，计算sample方法得到该出射方向的概率密度
•eval(constVector3f wi,constVector3fwo,constVector3fN)inMaterial.cpp:
给定一对入射、出射方向与法向量，计算这种情况下的f_r值

可能用到的变量有：
•RussianRouletteinScene.cpp:P_RR,RussianRoulette的概率

##~2先读修改部分：
1.mian多定义了一些参数加到scene参数里
2.render.cpp定义int spp参数控制每像素多次采样次数
3.和说明一样增加了area参数，sample函数和getsample函数
4.m_color和getColor()部分被注释了，多了hasEmission的布尔值，sample,pdf,eval函数，
在可能用到的函数里也提到了
##~2复制函数部分：
三个都有可以原样复制
const std::array<int, 3>& dirIsNeg) in the Bounds3.hpp要注意
tmin = tmax时要返回true,不然渲染不出来

##~2分析castRay函数：
返回向量，传入光线有关参数和深度，在Render.cpp的Render函数调用
用的是Ray Generation的结构
要实现路径追踪，先读PDF的伪代码
//伪代码中p,x,ws,NN,N分别对应图22的x,x',ω,n',n,emit对应发光颜色，//参数解析部分参考了zhen大佬的文章https://zhuanlan.zhihu.com/p/1898011358450161274?share_code=1639rdJPFPKw8&utm_psn=2090937868092970272

wo是p出射向眼睛的光,wi是相对应的入射光
shade(p, wo)
sampleLight(inter, pdf_light)
Get x, ws, NN, emit from inter
Shoot a ray from p to x
If the ray is not blocked in the middle
L_dir = emit * eval(wo, ws, N) * dot(ws, N) * dot(ws,
NN) / |x-p|^2 / pdf_light
L_indir = 0.0
Test Russian Roulette with probability RussianRoulette
wi = sample(wo, N)
Trace a ray r(p, wi)
If ray r hit a non-emitting object at q
L_indir = shade(q, wi) * eval(wo, wi, N) * dot(wi, N)
/ pdf(wo, wi, N) / RussianRoulette
Return L_dir + L_indir

分析：先samplelight获取光源信息，进而获取x,wx,NN,emit
对ws进行检测是否撞上物体，未撞上计算L_dir计算直接光
非直接光先进行俄罗斯轮盘赌再计算，
如果是由非光源部分q反射的光，继续递归轮盘赌计算非直接光
//如果是光源或未命中没提，但可以推得应该直接L_indir = 0；
最后返回L_dir+Lindir；

//由于samplelight传入的参数也是修改的参数，所以需要新创建一个Intersection防止p点坐标丢失
按伪代码思路设置参数翻译代码，
检测是否撞上物体，用ws光线的distance和p,x距离相比相等就不遮挡
未相交用happend获取，是否发光用hasEmission()函数获取
判断，计算，伪代码翻译完成
渲染
崩溃
原因：没有对最开始传入的ray的intersection进行检测是否happend
改，渲染
跑了142s，
不对，结果纯黑
排查
bound3的ntersectP函数里的tmin<tmax严格小于使与光线平行部分（tmin = tmax）不渲染,改成<=
成功，但跑了17min，1068s

##~2附加部分
查：
1.改用realse编译，Rider可以在顶部栏中切换Cmake配置文件，
从Debug改成realse（如果没有手动创建）
改完：
spp = 16,4min,275s
spp = 64,22min,1324s
2.多线程
渲染时函数调用顺序
Render → castRay → intersect → BVHAccel::Intersect → BVHAccel::getIntersection→ MeshTriangle::getIntersection → Triangle::getIntersection → 返回 Intersection
几乎每个像素点都几乎没有什么共用数据，除了随机数函数和buffer的计数参数m，修改一下可以并行
修改global.hpp的随机数函数
inline float get_random_float()
{
    static thread_local std::mt19937 rng(std::random_device{}());
    static thread_local std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(rng);
}//原版多线程的时候random的dev会挤在一起，thread_local 保证每线程一份。
修改m参数部分，删掉m的定义，需要调用的时候换成行和列相乘现场计算：j * scene.width + i
MSVC 的 OpenMP 2.0 只吃有符号整型，所以for循环j时要把uint改成int
最后把UpdateProgress(j / (float)scene.height);前面加上#pragma omp critical保证进度条正常
并行后：
spp = 16,29s
spp = 64,104s
spp = 256,8min,491s
##~1BIHW8
阅读要求：
你应该修改的函数是:
• rope.cpp 中的 Rope::rope(...)//构建绳子
• rope.cpp 中的 void Rope::simulateEuler(...)//显式/半隐式欧拉法
• rope.cpp 中的 void Rope::simulateVerlet(...)//显式 Verlet法
仔细阅读翻译代码
##~2Rope::rope:
翻spring.h和rope.h，阅读参数
pinner是表示是否固定的参数
遍历循环分别构建mass和spring
最后取消注释赋值pinned为true
##~2void Rope::simulateEuler(...):
根据PDF公式翻译并存到每个mass的force参数//注意力的方向
对没有pinned的mass带公式计算//人两种方法只选一个
实现阻尼部分要求没提，但需要在velocity参数上修改（每次force都会清零）
##~2void Rope::simulateVerlet(...)：
阅读翻译代码，构建临时向量参数存储，
这里要使用last_position来进行计算，每次计算完记得更新last_position
运行，成功




