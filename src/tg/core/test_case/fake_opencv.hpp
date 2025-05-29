#pragma once

namespace tg::core::test_case::fake_opencv
{
    struct Size
    {
        int width;
        int height;
    };

    class Mat
    {
    public:
        Size size() const
        {
            return Size{640, 480}; // Example size
        }
        int type() const
        {
            return 16; // Example type, e.g., CV_8UC3
        }
        Mat(const Size& size, int type)
        {
        }
    };

    inline void GaussianBlur(const Mat& src, Mat& dst)
    {}
};
