#include <vector>
#include <string>
#include "Vector/Vector3.cpp"
class Image
{
    private:
    int width, height;
    std::vector<Vector3d> buffer;

    public:
    Image(int width, int height);

    void SetPixel(int x, int y, const Vector3d& color);

    void SaveAsPBM(const std::string &filePath);
};