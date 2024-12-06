// #include "MarketPanel.hpp"

// MarketPanel::MarketPanel(float width, float height)
//     : panel(width, height, "Market Overview") {
//     if (!font.loadFromFile("../assets/fonts/font.ttf")) {
//         throw std::runtime_error("Failed to load font!");
//     }

//     // Set up graph title
//     graphTitle.setFont(font);
//     graphTitle.setCharacterSize(18);
//     graphTitle.setFillColor(sf::Color::White);
//     graphTitle.setString("Price Trends");
//     graphTitle.setPosition(panel.getBounds().left + 10, panel.getBounds().top + 10);

//     // Set up market details text
//     marketDetailsText.setFont(font);
//     marketDetailsText.setCharacterSize(14);
//     marketDetailsText.setFillColor(sf::Color::White);
//     marketDetailsText.setPosition(panel.getBounds().left + width / 2 + 10, panel.getBounds().top + 50);

//     // Initialize graph colors
//     graphColors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Cyan };
// }

// void MarketPanel::updateMarketDetails(const Market& market) {
//     std::ostringstream detailsStream;

//     detailsStream << "Market Details:\n";
//     detailsStream << "-------------------------\n";

//     for (const auto& [item, price] : market.getPriceTrendMap()) {
//         detailsStream << "Item: " << item << "\n";
//         detailsStream << "  Buy Price: $" << market.calculateBuyPrice(item) << "\n";
//         detailsStream << "  Sell Price: $" << market.calculateSellPrice(item) << "\n";
//         detailsStream << "  Demand: " << market.getDemand(item) << "\n";
//         detailsStream << "  Supply: " << market.getSupply(item) << "\n\n";
//     }

//     marketDetailsText.setString(detailsStream.str());
// }

// void MarketPanel::drawMarketGraph(sf::RenderWindow& window, const Market& market) {
//     float graphWidth = panel.getBounds().width / 2 - 20;
//     float graphHeight = panel.getBounds().height - 60;
//     sf::Vector2f graphPosition = { panel.getBounds().left + 10, panel.getBounds().top + 50 };

//     sf::RectangleShape graphBackground(sf::Vector2f(graphWidth, graphHeight));
//     graphBackground.setPosition(graphPosition);
//     graphBackground.setFillColor(sf::Color(30, 30, 30, 200));
//     window.draw(graphBackground);

//     float maxPrice = 1.0f;
//     for (const auto& [item, prices] : market.getPriceTrendMap()) {
//         if (!prices.empty()) {
//             maxPrice = std::max(maxPrice, *std::max_element(prices.begin(), prices.end()));
//         }
//     }

//     if (maxPrice == 0.0f) maxPrice = 1.0f;

//     int colorIndex = 0;
//     for (const auto& [item, prices] : market.getPriceTrendMap()) {
//         if (prices.empty()) continue;

//         sf::VertexArray line(sf::LineStrip, prices.size());
//         for (size_t i = 0; i < prices.size(); ++i) {
//             float x = graphPosition.x + (static_cast<float>(i) / (prices.size() - 1)) * graphWidth;
//             float y = graphPosition.y + graphHeight - (prices[i] / maxPrice) * graphHeight;
//             line[i].position = {x, y};
//             line[i].color = graphColors[colorIndex % graphColors.size()];
//         }

//         window.draw(line);

//         // Add item label
//         sf::Text label(item, font, 12);
//         label.setPosition(graphPosition.x + 5, graphPosition.y + graphHeight + 5 + (15 * colorIndex));
//         label.setFillColor(graphColors[colorIndex % graphColors.size()]);
//         window.draw(label);

//         colorIndex++;
//     }
// }

// void MarketPanel::handleEvent(sf::RenderWindow& window, sf::Event& event) {
//     panel.handleEvent(window, event);
// }

// void MarketPanel::render(sf::RenderWindow& window, const Market& market) {
//     panel.render(window);
//     window.draw(graphTitle);
//     drawMarketGraph(window, market);
//     window.draw(marketDetailsText);
// }
