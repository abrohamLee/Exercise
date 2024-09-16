#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>

struct TradeData {
    std::vector<long long int> timestamps;
    long long int volume = 0;
    long long int totalPriceTimesQuantity = 0;
    long long int maxPrice = 0;
};

class TradeAnalyzer {
public:
    void processInput(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream inputFile(inputFileName);
        if (!inputFile) {
            std::cerr << "Error opening input file." << std::endl;
            return;
        }

        std::string line;
        std::unordered_map<std::string, TradeData> symbolData;

        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string timestampStr, symbol, quantityStr, priceStr;
            if (std::getline(iss, timestampStr, ',') &&
                std::getline(iss, symbol, ',') &&
                std::getline(iss, quantityStr, ',') &&
                std::getline(iss, priceStr)) {
                try {
                    long long int timestamp = std::stoll(timestampStr);
                    long long int quantity = std::stoll(quantityStr);
                    long long int price = std::stoll(priceStr);

                    symbolData[symbol].timestamps.push_back(timestamp);
                    symbolData[symbol].volume += quantity;
                    symbolData[symbol].totalPriceTimesQuantity += quantity * price;
                    symbolData[symbol].maxPrice = std::max(symbolData[symbol].maxPrice, price);
                } catch (const std::exception& e) {
                    std::cerr << "Error processing line: " << line << ". Reason: " << e.what() << std::endl;
                }
            }
        }

        inputFile.close();
        calculateMaxTimeGaps(symbolData);
        generateOutput(symbolData, outputFileName);
    }

    void calculateMaxTimeGaps(std::unordered_map<std::string, TradeData>& symbolData) {
        for (auto& pair : symbolData) {
            if (pair.second.timestamps.size() > 1) {
                long long int maxGap = 0;
                for (size_t i = 1; i < pair.second.timestamps.size(); ++i) {
                    long long int gap = pair.second.timestamps[i] - pair.second.timestamps[i - 1];
                    maxGap = std::max(maxGap, gap);
                }
                pair.second.timestamps.clear(); // Clear timestamps after calculation
                pair.second.timestamps.push_back(maxGap);
            } else {
                pair.second.timestamps.push_back(0);
            }
        }
    }

    void generateOutput(const std::unordered_map<std::string, TradeData>& symbolData, const std::string& outputFileName) {
        std::set<std::string> sortedSymbols;
        for (const auto& pair : symbolData) {
            sortedSymbols.insert(pair.first);
        }

        std::ofstream outputFile(outputFileName);
        if (!outputFile) {
            std::cerr << "Error opening output file." << std::endl;
            return;
        }

        for (const std::string& symbol : sortedSymbols) {
            const TradeData& data = symbolData.at(symbol);
            long long int weightedAveragePrice = data.volume > 0? data.totalPriceTimesQuantity / data.volume : 0;
            outputFile << symbol << "," << data.timestamps[0] << "," << data.volume << "," << weightedAveragePrice << "," << data.maxPrice << std::endl;
        }

        outputFile.close();
    }
};
int main(int argc, char* argv[]) {
    if (argc!= 3) {
        std::cerr << "Usage: " << argv[0] << " input_file output_file" << std::endl;
        return 1;
    }

    TradeAnalyzer analyzer;
    analyzer.processInput(argv[1], argv[2]);

    return 0;
}
