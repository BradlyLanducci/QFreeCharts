#include "Series.h"
#include "../Shaders/RotationNode.h"

#include <QPair>
#include <QQuickWindow>
#include <chrono>

static inline uint64_t now()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(
						 std::chrono::high_resolution_clock::now().time_since_epoch())
			.count();
}

//----------------------------------------------------------------------------------------------------------------------------------//

Series::Series(QQuickItem* parent)
		: QQuickItem(parent)
{
	mp_timer = new QTimer(this);
	QObject::connect(
			mp_timer, &QTimer::timeout, mp_timer,
			[this]()
			{
				uint64_t begin = now();
				QVariantList points;
				float frequency = 0.1f; // Adjust this for the wave frequency
				float amplitude = 1.0;	// Adjust this for the wave amplitude

				// Generate points for a sine wave
				for (qsizetype i = 0; i < 1000; i++)
				{
					float x = i * frequency + phase;	 // Adjust the frequency for speed
					float y = amplitude * std::sin(x); // Calculate sine value
					points.append((y + 1.f) / 2.f);
					// points.append(dis(gen));
				}

				// Update the phase for the next update (to create movement)
				phase += 0.1; // Increment the phase to shift the sine wave over time

				replaceSeries(points);
				uint64_t end = now();
				// qDebug() << end - begin << "microseconds";
			},
			Qt::DirectConnection);

	mp_timer->setInterval(5);
	mp_timer->start();

	setFlag(ItemHasContents, true);
	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

Series::~Series()
{
	mp_timer->stop();
}

//----------------------------------------------------------------------------------------------------------------------------------//

void Series::addPoint(qreal y)
{
	m_points.pop_back();
	m_points.push_front(y);

	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

void Series::replaceSeries(QVariantList points)
{
	m_points = points;

	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

QSGNode* Series::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
	RotationNode* node = nullptr;
	QSGGeometry* geometry = nullptr;

	qsizetype pointsSize = m_points.size();
	const int numberOfCircleSegments = 12;
	const int totalVerticesPerPoint = 6 + (numberOfCircleSegments * 3) + 3; // 6 vertices (2 triangles per rectangle)

	if (pointsSize != 0)
	{
		if (!oldNode)
		{
			node = new RotationNode();

			static QSGGeometry::Attribute attributes[] = {
					QSGGeometry::Attribute::create(0, 2, QSGGeometry::FloatType, true), // Position
					QSGGeometry::Attribute::create(1, 1, QSGGeometry::FloatType),				// Theta
					QSGGeometry::Attribute::create(2, 2, QSGGeometry::FloatType, true)	// rotationPoint
			};
			static QSGGeometry::AttributeSet set;
			set.count = 3;
			set.stride = 5 * sizeof(float);
			set.attributes = attributes;

			geometry = new QSGGeometry(set, pointsSize * totalVerticesPerPoint);
			geometry->setLineWidth(1); // Graphics API enforces line width of 1
			geometry->setDrawingMode(QSGGeometry::DrawTriangles);
			node->setGeometry(geometry);
			node->setFlag(QSGNode::OwnsGeometry);
		}
		else
		{
			node = static_cast<RotationNode*>(oldNode);
			geometry = node->geometry();
			geometry->allocate(pointsSize * totalVerticesPerPoint);
		}

		// Access raw vertex data
		float* vertexData = reinterpret_cast<float*>(geometry->vertexData());

		// Scale the x-axis based on the number of points
		float xWidth = width() / (pointsSize - 1);

		for (int i = 0; i < pointsSize - 1; i++)
		{
			// Coordinates for p1 (x_i) and p2 (x_i+1)
			float x1 = i * xWidth;
			float y1 = m_points[i].toFloat() * height();
			float x2 = (i + 1) * xWidth;
			float y2 = m_points[i + 1].toFloat() * height();

			// Line thickness
			float lineWidth = 4.0;
			float halfLineWidth = lineWidth / 2.0;

			// Calculate rotation (theta) between points p1 and p2
			float w = x2 - x1;
			float h = y2 - y1;
			float diagonal = std::sqrt(w * w + h * h);

			float theta = std::acos(w / diagonal);

			// Theta is inverted when the point is going down
			// Also maybe the whole y axis needs to be flipped
			if (y2 > y1)
			{
				theta = -theta;
			}

			float x2Modified = x1 + diagonal;

			// Write vertex data (positions, theta, and center)
			int verticeIndex = i * totalVerticesPerPoint *
												 5; // 5 floats per vertex (x, y, theta, centerX, centerY) change center to be rotationXY

			// Maybe set theta + rotation point programataically?
			vertexData[verticeIndex] = x1;
			vertexData[verticeIndex + 1] = y1 - halfLineWidth;
			vertexData[verticeIndex + 2] = theta;
			vertexData[verticeIndex + 3] = x1;
			vertexData[verticeIndex + 4] = y1;

			vertexData[verticeIndex + 5] = x1;
			vertexData[verticeIndex + 6] = y1 + halfLineWidth;
			vertexData[verticeIndex + 7] = theta;
			vertexData[verticeIndex + 8] = x1;
			vertexData[verticeIndex + 9] = y1;

			vertexData[verticeIndex + 10] = x2Modified;
			vertexData[verticeIndex + 11] = y1 - halfLineWidth;
			vertexData[verticeIndex + 12] = theta;
			vertexData[verticeIndex + 13] = x1;
			vertexData[verticeIndex + 14] = y1;

			vertexData[verticeIndex + 15] = x2Modified;
			vertexData[verticeIndex + 16] = y1 + halfLineWidth;
			vertexData[verticeIndex + 17] = theta;
			vertexData[verticeIndex + 18] = x1;
			vertexData[verticeIndex + 19] = y1;

			vertexData[verticeIndex + 20] = x1;
			vertexData[verticeIndex + 21] = y1 + halfLineWidth;
			vertexData[verticeIndex + 22] = theta;
			vertexData[verticeIndex + 23] = x1;
			vertexData[verticeIndex + 24] = y1;

			vertexData[verticeIndex + 25] = x2Modified;
			vertexData[verticeIndex + 26] = y1 - halfLineWidth;
			vertexData[verticeIndex + 27] = theta;
			vertexData[verticeIndex + 28] = x1;
			vertexData[verticeIndex + 29] = y1;

			// Place circle within rest of vertices (need to modify total amount to reflect the (circle segments + 1) * 3
			// extra vertices

			// Construct circle at x2 unmodified
			// Create circle connecting rectangles, we need 1 extra segment due to it needing to finish connecting the circle
			// together
			std::vector<QPair<float, float>> circlePoints(numberOfCircleSegments + 1);
			const float circleRadius = halfLineWidth;
			std::size_t index = 0;
			for (auto& circlePoint : circlePoints)
			{
				float angle = 2.0 * M_PI * static_cast<float>(index) / static_cast<float>(numberOfCircleSegments);

				circlePoint.first = circleRadius * std::cos(angle);
				circlePoint.second = circleRadius * std::sin(angle);

				index++;
			}

			// set to circle origin
			float originX = x2;
			float originY = y2;

			// Would rather name this something else for cleaner code
			index = 0;
			for (int j = 40; j < totalVerticesPerPoint * 5; j += 15)
			{
				// Draw segment of circle
				vertexData[verticeIndex + j] = originX;
				vertexData[verticeIndex + j + 1] = originY;
				vertexData[verticeIndex + j + 2] = theta;
				vertexData[verticeIndex + j + 3] = x2;
				vertexData[verticeIndex + j + 4] = y2;

				vertexData[verticeIndex + j + 5] = originX + circlePoints[index].first;
				vertexData[verticeIndex + j + 6] = originY + circlePoints[index].second;
				vertexData[verticeIndex + j + 7] = theta;
				vertexData[verticeIndex + j + 8] = x2;
				vertexData[verticeIndex + j + 9] = y2;

				vertexData[verticeIndex + j + 10] = originX + circlePoints[index + 1].first;
				vertexData[verticeIndex + j + 11] = originY + circlePoints[index + 1].second;
				vertexData[verticeIndex + j + 12] = theta;
				vertexData[verticeIndex + j + 13] = x2;
				vertexData[verticeIndex + j + 14] = y2;

				index++;
			}

			// Return to x_i+1 in preparation for next iteration
			int lastIndex = totalVerticesPerPoint * 5;

			vertexData[verticeIndex + lastIndex] = originX;
			vertexData[verticeIndex + lastIndex + 1] = originY;
			vertexData[verticeIndex + lastIndex + 2] = theta;
			vertexData[verticeIndex + lastIndex + 3] = x2;
			vertexData[verticeIndex + lastIndex + 4] = y2;
		}

		node->markDirty(QSGNode::DirtyGeometry);
	}

	return node;
}

//----------------------------------------------------------------------------------------------------------------------------------//
