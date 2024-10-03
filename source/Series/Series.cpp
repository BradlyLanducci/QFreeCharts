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
		, m_color("Black")
		, m_lineWidth(1.f)
{
	setFlag(ItemHasContents, true);
	setClip(true);
	setAntialiasing(false);

	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

Series::~Series() = default;

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

QColor Series::color() const
{
	return m_color;
}

//----------------------------------------------------------------------------------------------------------------------------------//

float Series::lineWidth() const
{
	return m_lineWidth;
}

//----------------------------------------------------------------------------------------------------------------------------------//

void Series::setColor(QColor color)
{
	m_color = color;
}

//----------------------------------------------------------------------------------------------------------------------------------//

void Series::setLineWidth(float lineWidth)
{
	m_lineWidth = lineWidth;
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
			RGBA colorVector4d = QVector4D(m_color.redF(), m_color.greenF(), m_color.blueF(), m_color.alphaF());
			node = new RotationNode(colorVector4d);

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
			node->setColor(m_color);
		}

		// Access raw vertex data
		float* vertexData = reinterpret_cast<float*>(geometry->vertexData());

		// Scale the x-axis based on the number of points
		float xWidth = width() / (pointsSize - 1);

		for (int i = 0; i < pointsSize - 1; i++)
		{
			// Coordinates for p1 (x_i) and p2 (x_i+1) and inverted y axis
			float x1 = i * xWidth;
			float y1 = (1.f - m_points[i].toFloat()) * height();
			float x2 = (i + 1) * xWidth;
			float y2 = (1.f - m_points[i + 1].toFloat()) * height();

			// Line thickness
			float lineWidth = m_lineWidth;
			float halfLineWidth = lineWidth / 2.0;

			// Calculate rotation (theta) between points p1 and p2
			float w = x2 - x1;
			float h = y2 - y1;
			float diagonal = std::sqrt(w * w + h * h);

			float theta = std::acos(w / diagonal);

			// Theta is inverted when the point is going down
			if (y2 > y1)
			{
				theta = -theta;
			}

			float x2Modified = x1 + diagonal;

			// Write vertex data (5 floats: positions, theta, and rotation point)
			int verticeIndex = i * totalVerticesPerPoint * 5;

			for (int i = 0; i < 40; i += 5)
			{
				vertexData[verticeIndex + i + 2] = theta;
				vertexData[verticeIndex + i + 3] = x1;
				vertexData[verticeIndex + i + 4] = y1;
			}

			// Draw vertices so that we end up on the right side of the rectangle
			vertexData[verticeIndex] = x1;
			vertexData[verticeIndex + 1] = y1 - halfLineWidth;
			vertexData[verticeIndex + 5] = x1;
			vertexData[verticeIndex + 6] = y1 + halfLineWidth;
			vertexData[verticeIndex + 10] = x2Modified;
			vertexData[verticeIndex + 11] = y1 - halfLineWidth;
			vertexData[verticeIndex + 15] = x2Modified;
			vertexData[verticeIndex + 16] = y1 + halfLineWidth;
			vertexData[verticeIndex + 20] = x1;
			vertexData[verticeIndex + 21] = y1 + halfLineWidth;
			vertexData[verticeIndex + 25] = x2Modified;
			vertexData[verticeIndex + 26] = y1 - halfLineWidth;

			// Create circle connecting rectangles
			std::vector<QPair<float, float>> circlePoints(numberOfCircleSegments + 1);
			const float circleRadius = halfLineWidth;
			std::size_t circleIndex = 0;
			for (auto& circlePoint : circlePoints)
			{
				float angle = 2.f * M_PI * static_cast<float>(circleIndex) / static_cast<float>(numberOfCircleSegments);

				circlePoint.first = circleRadius * std::cos(angle);
				circlePoint.second = circleRadius * std::sin(angle);

				circleIndex++;
			}

			// Set to circle origin
			float originX = x2;
			float originY = y2;

			circleIndex = 0;
			for (int j = 30; j < totalVerticesPerPoint * 5; j += 15)
			{
				// Draw segment of circle
				vertexData[verticeIndex + j] = originX;
				vertexData[verticeIndex + j + 1] = originY;
				vertexData[verticeIndex + j + 2] = theta;
				vertexData[verticeIndex + j + 3] = x2;
				vertexData[verticeIndex + j + 4] = y2;

				vertexData[verticeIndex + j + 5] = originX + circlePoints[circleIndex].first;
				vertexData[verticeIndex + j + 6] = originY + circlePoints[circleIndex].second;
				vertexData[verticeIndex + j + 7] = theta;
				vertexData[verticeIndex + j + 8] = x2;
				vertexData[verticeIndex + j + 9] = y2;

				vertexData[verticeIndex + j + 10] = originX + circlePoints[circleIndex + 1].first;
				vertexData[verticeIndex + j + 11] = originY + circlePoints[circleIndex + 1].second;
				vertexData[verticeIndex + j + 12] = theta;
				vertexData[verticeIndex + j + 13] = x2;
				vertexData[verticeIndex + j + 14] = y2;

				circleIndex++;
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
