#include <vector>
#include <string>
#include "../Vector/Vector3.cpp"
class Image
{
public:
    int width, height;

private:
    std::vector<Vector3d> buffer;

public:
    Image(int width, int height);

    void SetPixel(int x, int y, const Vector3d& color);

    void SaveAsPBM(const std::string &filepath, const std::string &filename);
};