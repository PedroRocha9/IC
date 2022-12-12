#include <string>
#include <fstream>
#include <regex>

class YUV4MPEG2Reader {
public:
  // Constructs a YUV4MPEG2Reader that reads from the given file.
  // Throws an exception if the file cannot be opened.
  YUV4MPEG2Reader(const std::string& file_name)
    : file_(file_name, std::ios::in | std::ios::binary) {
    if (!file_.is_open()) {
      throw std::runtime_error("Cannot open file: " + file_name);
    }

    // Parse the header of the file to extract the width, height, and color space.
    std::string line;
    // read first line
    std::getline(file_, line);

    // extract width and height
    sscanf(line.c_str(), "YUV4MPEG2 W%d H%d", &width_, &height_);
    // check if 'C' is in the line
    if (strchr(line.c_str(), 'C') == NULL) {
      color_space_ = "420";
    } else {
      // extract only the color space started by 'C'
      color_space_ = line.substr(line.find('C') + 1);
      std::cout << "here" << std::endl;
    }
  }

  // Returns the width of the frame.
  int width() const {
    return width_;
  }

  // Returns the height of the frame.
  int height() const {
    return height_;
  }

  // Returns the color space defined in the header (parameter 'C').
  std::string colorSpace() const {
    return color_space_;
  }

private:
  std::ifstream file_;
  int width_;
  int height_;
  std::string color_space_;
};
