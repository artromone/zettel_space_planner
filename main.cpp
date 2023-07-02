#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <cmath>
#include <conio.h>
template< typename TP >
std::time_t to_time_t(TP tp)
{
  using namespace std::chrono;
  auto sctp = time_point_cast< system_clock::duration >(tp - TP::clock::now() + system_clock::now());
  return system_clock::to_time_t(sctp);
}
void calculateFileStatistics(const std::filesystem::path &currentDir, const std::vector< std::string > &extensions,
                             int &fileCount, double &totalSize, std::time_t &oldestFileDate,
                             std::time_t &newestFileDate)
{
  for (const auto &entry: std::filesystem::recursive_directory_iterator(currentDir))
  {
    if (entry.is_regular_file())
    {
      const auto &filePath = entry.path();
      const std::string fileExtension = filePath.extension().string();
      if (std::find(extensions.begin(), extensions.end(), fileExtension) != extensions.end())
      {
        const double fileSize = std::filesystem::file_size(filePath);
        const std::time_t fileDate = to_time_t(std::filesystem::last_write_time(filePath));
        fileCount++;
        totalSize += fileSize;
        oldestFileDate = std::min(oldestFileDate, fileDate);
        newestFileDate = std::max(newestFileDate, fileDate);
      }
    }
  }
}
int calculateDateDifference(std::time_t oldestFileDate, std::time_t newestFileDate)
{
  std::chrono::system_clock::time_point oldestTimePoint = std::chrono::system_clock::from_time_t(oldestFileDate);
  std::chrono::system_clock::time_point newestTimePoint = std::chrono::system_clock::from_time_t(newestFileDate);
  std::chrono::duration< double > diff = newestTimePoint - oldestTimePoint;
  return std::chrono::duration_cast< std::chrono::hours >(diff).count() / 24;
}
double printResults(int fileCount, double totalSize, int numberOfDays, const std::string &title, double years)
{
  double averageSize = (fileCount > 0) ? (totalSize / fileCount) : 0.0;
  double averageSizeMB = averageSize / (1024.0 * 1024.0);
  int daysInYear = 365;
  double totalSizeMB = fileCount * averageSizeMB * years * daysInYear / numberOfDays;
  std::cout << "Number of " << title << ": " << fileCount << "\n";
  std::cout << "Average file size: " << averageSizeMB << " MB\n";
  std::cout << "Total space required for " << years << " years of " << title << ": " << totalSizeMB << " MB\n\n";
  return totalSizeMB;
}
int main()
{
  std::filesystem::path currentDir = std::filesystem::current_path();
  int mdFileCount = 0;
  double mdTotalSize = 0.0;
  std::time_t oldestFileDate = std::numeric_limits< std::time_t >::max();
  std::time_t newestFileDate = std::time_t(0);
  int imageFileCount = 0;
  double imageTotalSize = 0.0;
  std::vector< std::string > mdExtensions = {".md"};
  std::vector< std::string > imageExtensions = {".png", ".jpg", ".jpeg"};
  calculateFileStatistics(currentDir, mdExtensions, mdFileCount, mdTotalSize, oldestFileDate, newestFileDate);
  calculateFileStatistics(currentDir, imageExtensions, imageFileCount, imageTotalSize, oldestFileDate,
                          newestFileDate);
  std::cout << "Oldest file: " << std::put_time(std::localtime(&oldestFileDate), "%x %X") << "\n";
  std::cout << "Newest file: " << std::put_time(std::localtime(&newestFileDate), "%x %X") << "\n";
  int numberOfDays = calculateDateDifference(oldestFileDate, newestFileDate);
  std::cout << "Number of days between the dates: " << numberOfDays << "\n\n";
  double totalSizeMB = 0.0;
  double years = 5.0;
  totalSizeMB += printResults(mdFileCount, mdTotalSize, numberOfDays, "notes", years);
  totalSizeMB += printResults(imageFileCount, imageTotalSize, numberOfDays, "assets", years);
  double totalSizeGB = std::ceil(totalSizeMB / 1024.0 / years * 100);
  std::cout << "Total space required for 100 years (lifetime) of notes and images: " << totalSizeGB << " GB\n\n";
  std::cout << "Press any key to continue...\n";
  _getch();
  return 0;
}
