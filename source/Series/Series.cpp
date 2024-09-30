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
				float frequency = 0.0001f; // Adjust this for the wave frequency
				float amplitude = 1.0;		 // Adjust this for the wave amplitude

				// Generate points for a sine wave
				for (qsizetype i = 0; i < 1000000; i++)
				{
					float x = i * frequency + phase;	 // Adjust the frequency for speed
					float y = amplitude * std::sin(x); // Calculate sine value
					points.append((y + 1.f) / 2.f);
					// points.append(dis(gen));
				}

				// Update the phase for the next update (to create movement)
				phase += 0.1; // Increment the phase to shift the sine wave over tim

				replaceSeries(points);
				uint64_t end = now();
				// qDebug() << end - begin << "microseconds";
			},
			Qt::DirectConnection);

	mp_timer->setInterval(50);
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
	const int totalVerticesPerPoint = 6; // 6 vertices (2 triangles per rectangle)

	if (pointsSize != 0)
	{
		if (!oldNode)
		{
			node = new RotationNode();

			// Custom attributes: position (vec2), theta (float), and center (vec2)
			static QSGGeometry::Attribute attr[] = {
					QSGGeometry::Attribute::create(0, 2, QSGGeometry::FloatType, true), // Position
					QSGGeometry::Attribute::create(1, 1, QSGGeometry::FloatType),				// Theta
					QSGGeometry::Attribute::create(2, 2, QSGGeometry::FloatType, true)	// Center
			};
			static QSGGeometry::AttributeSet set = {3, 5 * sizeof(float), attr};

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

			float theta;

			theta = std::acos(w / diagonal);

			if (y2 > y1)
			{
				theta = -theta;
			}

			x2 = x1 + diagonal;

			// Write vertex data (positions, theta, and center)
			int verticeIndex = i * totalVerticesPerPoint * 5; // 5 floats per vertex (x, y, theta, centerX, centerY)

			// Vertices for two triangles forming a rectangle
			vertexData[verticeIndex] = x1;										 // x1
			vertexData[verticeIndex + 1] = y1 - halfLineWidth; // y1
			vertexData[verticeIndex + 2] = theta;							 // rotation angle (theta)
			vertexData[verticeIndex + 3] = x1;								 // centerX
			vertexData[verticeIndex + 4] = y1;								 // centerY

			vertexData[verticeIndex + 5] = x1;								 // x1
			vertexData[verticeIndex + 6] = y1 + halfLineWidth; // y2
			vertexData[verticeIndex + 7] = theta;							 // theta
			vertexData[verticeIndex + 8] = x1;								 // centerX
			vertexData[verticeIndex + 9] = y1;								 // centerY

			vertexData[verticeIndex + 10] = x2;									// x2
			vertexData[verticeIndex + 11] = y1 - halfLineWidth; // y1
			vertexData[verticeIndex + 12] = theta;							// theta
			vertexData[verticeIndex + 13] = x1;									// centerX
			vertexData[verticeIndex + 14] = y1;									// centerY

			vertexData[verticeIndex + 15] = x2;									// x2
			vertexData[verticeIndex + 16] = y1 + halfLineWidth; // y2
			vertexData[verticeIndex + 17] = theta;							// theta
			vertexData[verticeIndex + 18] = x1;									// centerX
			vertexData[verticeIndex + 19] = y1;									// centerY

			vertexData[verticeIndex + 20] = x1;									// x1
			vertexData[verticeIndex + 21] = y1 + halfLineWidth; // y2
			vertexData[verticeIndex + 22] = theta;							// theta
			vertexData[verticeIndex + 23] = x1;									// centerX
			vertexData[verticeIndex + 24] = y1;									// centerY

			vertexData[verticeIndex + 25] = x2;									// x2
			vertexData[verticeIndex + 26] = y1 - halfLineWidth; // y1
			vertexData[verticeIndex + 27] = theta;							// theta
			vertexData[verticeIndex + 28] = x1;									// centerX
			vertexData[verticeIndex + 29] = y1;									// centerY

			// qDebug() << x1 << y1 << x2 << y2 << theta << lineWidth << diagonal;
		}

		node->markDirty(QSGNode::DirtyGeometry);
	}

	return node;
}

//----------------------------------------------------------------------------------------------------------------------------------//
