#include "PixelOperations.hpp"
#include <queue>
#include <thread>
#include "Constants.hpp"

b2Vec2 GetPixelRigidBodyPosition(sf::Vector2f position) {
	return {position.x / SCALE, position.y / SCALE};
}

Bounds ComputeBounds(const std::vector<Pixel>& pixels) {
    if (pixels.empty()) return {0, 0, 0, 0};

    Bounds b = {
        pixels[0].localPos.x, 
        pixels[0].localPos.x, 
        pixels[0].localPos.y, 
        pixels[0].localPos.y
    };

    for (const Pixel& p : pixels) {
        b.minX = std::min(b.minX, p.localPos.x);
        b.maxX = std::max(b.maxX, p.localPos.x);
        b.minY = std::min(b.minY, p.localPos.y);
        b.maxY = std::max(b.maxY, p.localPos.y);
    }
    return b;
}

std::vector<Rect> MergePixelsIntoRects(const std::vector<Pixel>& pixels) {
    Bounds b = ComputeBounds(pixels);
    int width = b.maxX - b.minX + 1;
    int height = b.maxY - b.minY + 1;
    
    std::vector<sf::Color> colorGrid(width * height, sf::Color::Transparent);
    for (const Pixel& p : pixels) {
        int x = p.localPos.x - b.minX;
        int y = p.localPos.y - b.minY;
        colorGrid[y * width + x] = p.color;
    }

    std::vector<Pixel> sorted = pixels;
    std::sort(sorted.begin(), sorted.end(), [](const Pixel& a, const Pixel& b) {
        return (a.localPos.y < b.localPos.y) || 
              (a.localPos.y == b.localPos.y && a.localPos.x < b.localPos.x);
    });

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::vector<bool>> threadUsed(numThreads, 
        std::vector<bool>(width * height, false));
    std::vector<std::vector<Rect>> threadRects(numThreads);
    
    auto processChunk = [&](int threadId, size_t start, size_t end) {
        std::vector<Rect>& rects = threadRects[threadId];
        std::vector<bool>& used = threadUsed[threadId];
        
        for (size_t i = start; i < end; i++) {
            const Pixel& p = sorted[i];
            int gridX = p.localPos.x - b.minX;
            int gridY = p.localPos.y - b.minY;
            
            if (used[gridY * width + gridX]) continue;
            
            const sf::Color targetColor = p.color;
            int startX = gridX;
            int startY = gridY;
            
			int maxRight = gridX;
            while (maxRight < width - 1 && 
                   colorGrid[gridY * width + (maxRight + 1)] == targetColor &&
                   !used[gridY * width + (maxRight + 1)]) {
                maxRight++;
            }
            
            int maxDown = gridY;
            while (maxDown < height - 1) {
                bool valid = true;
                for (int x = startX; x <= maxRight; x++) {
                    if (colorGrid[(maxDown + 1) * width + x] != targetColor ||
                        used[(maxDown + 1) * width + x]) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) break;
                maxDown++;
            }
            
            for (int y = startY; y <= maxDown; y++) {
                for (int x = startX; x <= maxRight; x++) {
                    used[y * width + x] = true;
                }
            }
            
            rects.emplace_back(Rect{
                sf::Vector2i(b.minX + startX, b.minY + startY),
                sf::Vector2i(maxRight - startX + 1, maxDown - startY + 1),
                targetColor
			});
        }
    };

    const size_t perThread = sorted.size() / numThreads;
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        size_t start = i * perThread;
        size_t end = (i == numThreads - 1) ? sorted.size() : (i + 1) * perThread;
        threads.emplace_back(processChunk, i, start, end);
    }
    for (auto& t : threads) t.join();

    std::vector<Rect> result;
    std::vector<bool> globalUsed(width * height, false);
    
    for (const auto& mask : threadUsed) {
        for (size_t i = 0; i < mask.size(); i++) {
            globalUsed[i] = globalUsed[i] || mask[i];
        }
    }
    
    for (auto& vec : threadRects) {
        result.insert(result.end(), vec.begin(), vec.end());
    }

    std::sort(result.begin(), result.end(), [](const Rect& a, Rect& b) {
        return std::tie(a.pos.y, a.pos.x) < std::tie(b.pos.y, b.pos.x);
    });

    std::vector<Rect> merged;
    for (const Rect& rect : result) {
        if (!merged.empty() && 
            merged.back().pos.y == rect.pos.y &&
            merged.back().pos.x + merged.back().size.x == rect.pos.x &&
            merged.back().size.y == rect.size.y &&
            merged.back().color == rect.color) {
            merged.back().size.x += rect.size.x;
        } else {
            merged.push_back(rect);
        }
    }

    return merged;
}

void GetPixelRectVertices(sf::VertexArray &localVertices, const Rect& rect) {
	float sx = rect.pos.x * PIXEL_SIZE;
	float sy = rect.pos.y * PIXEL_SIZE;
	float sw = rect.size.x * PIXEL_SIZE;
	float sh = rect.size.y * PIXEL_SIZE;

	sf::Vector2f corners[4] = {
		{sx, sy},
		{sx + sw, sy},
		{sx + sw, sy + sh},
		{sx, sy + sh}
	};

	sf::Vertex v0{corners[0], rect.color};
	sf::Vertex v1{corners[1], rect.color};
	sf::Vertex v2{corners[2], rect.color};
	sf::Vertex v3{corners[3], rect.color};

	localVertices.append(v0);
	localVertices.append(v1);
	localVertices.append(v2);
	localVertices.append(v0);
	localVertices.append(v2);
	localVertices.append(v3);
}

b2Vec2 GetRectSize(sf::Vector2i size) {
	return {
		size.x * PIXEL_SIZE / SCALE / 2.f,
		size.y * PIXEL_SIZE / SCALE / 2.f
	};
}

b2Vec2 GetRectCenter(sf::Vector2i position, sf::Vector2i size) {
	return {
		(position.x + size.x / 2.f) * PIXEL_SIZE / SCALE,
		(position.y + size.y / 2.f) * PIXEL_SIZE / SCALE
	};
}

std::vector<std::vector<Pixel>> FindConnectedComponents(const std::vector<Pixel>& pixels) {
    std::vector<std::vector<Pixel>> components;
    if (pixels.empty()) return components;

    Bounds b = ComputeBounds(pixels);
    int width = b.maxX - b.minX + 1;
    int height = b.maxY - b.minY + 1;

    std::unordered_set<sf::Vector2i, Vector2iHash> pixelSet;
    std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));

    // Mark all existing pixels
    for (const Pixel& p : pixels) {
        int x = p.localPos.x - b.minX;
        int y = p.localPos.y - b.minY;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixelSet.insert(p.localPos);
        }
    }

    // Flood fill to find connected components
    for (const Pixel& p : pixels) {
        sf::Vector2i pos = p.localPos;
        int gridX = pos.x - b.minX;
        int gridY = pos.y - b.minY;
        
        if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height || visited[gridX][gridY])
            continue;

        std::queue<sf::Vector2i> q;
        std::vector<Pixel> component;
        q.push(pos);
        visited[gridX][gridY] = true;

        while (!q.empty()) {
            sf::Vector2i current = q.front();
            q.pop();

            // Add to component
            component.push_back({current, p.color});

            // Check 4 neighbors
            const sf::Vector2i dirs[] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
            for (const auto& dir : dirs) {
                sf::Vector2i next(current.x + dir.x, current.y + dir.y);
                int nextX = next.x - b.minX;
                int nextY = next.y - b.minY;

                if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height &&
                    pixelSet.count(next) && !visited[nextX][nextY]) {
                    visited[nextX][nextY] = true;
                    q.push(next);
                }
            }
        }
        components.push_back(component);
    }
    return components;
}

b2Vec2 CalculateComponentCentroid(const std::vector<Pixel>& component) {
    float avgX = 0.0f, avgY = 0.0f;
    for (const Pixel& p : component) {
        avgX += p.localPos.x + 0.5f;
        avgY += p.localPos.y + 0.5f;
    }
    avgX /= component.size();
    avgY /= component.size();

    return {
        (avgX * PIXEL_SIZE) / SCALE,
        (avgY * PIXEL_SIZE) / SCALE
    };
}

std::vector<Pixel> AdjustPixelCoordinates(const std::vector<Pixel>& component, const b2Vec2& centroidPixelSpace) {
    sf::Vector2i baseOffset(
        static_cast<int>(std::floor(centroidPixelSpace.x)),
        static_cast<int>(std::floor(centroidPixelSpace.y))
    );

    std::vector<Pixel> newPixels;
    for (const Pixel& p : component) {
        newPixels.push_back({
            sf::Vector2i(p.localPos.x - baseOffset.x, 
                        p.localPos.y - baseOffset.y),
            p.color
        });
    }
    return newPixels;
}

std::vector<Pixel> ConvertImageToPixels(const std::string& filename, const sf::Color& ignoreColor)
{
	std::vector<Pixel> pixels;
    
    sf::Image image;
    if (!image.loadFromFile(filename)) {
        throw std::runtime_error("Failed to load image: " + filename);
    }
    
    sf::Vector2u size = image.getSize();
    pixels.reserve(size.x * size.y);
    
    for (unsigned y = 0; y < size.y; ++y) {
        for (unsigned x = 0; x < size.x; ++x) {
            sf::Color color = image.getPixel({x, y});
            
            if(color != ignoreColor) {
                pixels.push_back({{static_cast<int>(x), static_cast<int>(y)}, color});
            }
        }
    }
    
    return pixels;
}
