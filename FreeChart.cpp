#include "FreeChart.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPair>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

#include <chrono>

static inline uint64_t now()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(
						 std::chrono::high_resolution_clock::now().time_since_epoch())
			.count();
}

//----------------------------------------------------------------------------------------------------------------------------------//

FreeChart::FreeChart(QQuickItem* parent)
		: QQuickItem(parent)
{
	mp_timer = new QTimer(this);
	QObject::connect(
			mp_timer, &QTimer::timeout, mp_timer,
			[this]()
			{
				uint64_t begin = now();
				QVariantList points;
				double frequency = 0.1f; // Adjust this for the wave frequency
				double amplitude = 1.0;	 // Adjust this for the wave amplitude

				// Generate points for a sine wave
				for (qsizetype i = 0; i < 50; i++)
				{
					double x = i * frequency + phase;		// Adjust the frequency for speed
					double y = amplitude * std::sin(x); // Calculate sine value
					// points.append((y + 1.f) / 2.f);
					points.append(dis(gen));
				}

				// Update the phase for the next update (to create movement)
				phase += 0.1; // Increment the phase to shift the sine wave over tim

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

FreeChart::~FreeChart()
{
	mp_timer->stop();
}

//----------------------------------------------------------------------------------------------------------------------------------//

void FreeChart::addPoint(qreal y)
{
	m_points.pop_back();
	m_points.push_front(y);

	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

void FreeChart::replaceSeries(QVariantList points)
{
	m_points = points;

	update();
}

//----------------------------------------------------------------------------------------------------------------------------------//

QSGNode* FreeChart::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
	QSGGeometryNode* node = nullptr;
	QSGGeometry* geometry = nullptr;

	qsizetype pointsSize = m_points.size();

	// temp
	const int numCircleSegments = 12;
	const int numCircleVertices = numCircleSegments * 3;
	const int numRectangleVertices = 6;

	const int totalVerticesPerPoint =
			numCircleVertices + numRectangleVertices + 3; // Number of circle triangles + rectangle + last triangle for circle

	if (pointsSize != 0)
	{
		if (!oldNode)
		{
			node = new QSGGeometryNode();
			geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), pointsSize * totalVerticesPerPoint);
			geometry->setLineWidth(1); // Graphics API enforces line width of 1
			geometry->setDrawingMode(QSGGeometry::DrawTriangles);
			node->setGeometry(geometry);
			node->setFlag(QSGNode::OwnsGeometry);

			auto* material = new QSGFlatColorMaterial();
			material->setColor(QColor(0, 143, 17, 255));
			node->setMaterial(material);
			node->setFlag(QSGNode::OwnsMaterial);
		}
		else
		{
			node = static_cast<QSGGeometryNode*>(oldNode);
			geometry = node->geometry();
			geometry->allocate(pointsSize * totalVerticesPerPoint);
		}

		QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

		// We are filling this space, so we scale the x axis by the amount of points
		float xWidth = width() / (pointsSize - 1);

		for (int i = 0; i < pointsSize - 1; i++)
		{
			// p1 (x_i)
			double x1 = i * xWidth;
			double y1 = m_points[i].toDouble() * height();

			// p2 (x_i+1)
			double x2 = (i + 1) * xWidth;
			double y2 = m_points[i + 1].toFloat() * height();

			// Series line thickness
			double lineWidth = 2.0;

			// Line length connecting p1 and p2
			double diagonal = std::sqrt(std::pow(x2 - x1, 2.0) + std::pow(y2 - y1, 2.0));

			// Half of rectangles "height"
			double hypotenuse = lineWidth / 2;
			// Angle of rotation from xi to xi+1
			double theta = std::acos((x2 - x1) / diagonal);

			// Adjacent
			double s1 = hypotenuse * std::sin(theta);
			// Opposite
			double s2 = hypotenuse * std::cos(theta);

			// For indexing into vertices
			int verticeIndex = i * totalVerticesPerPoint;

			// Flip when the rectangle is going up (y axis is flipped in gui)
			if (y1 > y2)
			{
				s2 = -s2;
			}

			// Top left
			double v1x = x1 - s1;
			double v1y = y1 + s2;
			// Bottom left
			double v2x = x1 + s1;
			double v2y = y1 - s2;
			// Top right
			double v3x = x2 - s1;
			double v3y = y2 + s2;
			// Bottom right
			double v4x = x2 + s1;
			double v4y = y2 - s2;

			// Drawn in this specific order to end up with vertice on the correct side of the rectangle
			vertices[verticeIndex].set(v1x, v1y);
			vertices[verticeIndex + 1].set(v2x, v2y);
			vertices[verticeIndex + 2].set(v3x, v3y);
			vertices[verticeIndex + 3].set(v4x, v4y);
			vertices[verticeIndex + 4].set(v2x, v2y);
			vertices[verticeIndex + 5].set(v3x, v3y);

			// Create circle connecting rectangles, we need 1 extra segment due to it needing to finish connecting the circle
			// together
			std::vector<QPair<double, double>> circlePoints(numCircleSegments + 1);
			const double circleRadius = hypotenuse;
			std::size_t index = 0;
			for (auto& circlePoint : circlePoints)
			{
				double angle = 2.0 * M_PI * static_cast<double>(index) / static_cast<double>(numCircleSegments);

				circlePoint.first = circleRadius * std::cos(angle);
				circlePoint.second = circleRadius * std::sin(angle);

				index++;
			}

			// set to circle origin
			double originX = x2;
			double originY = y2;

			// Would rather name this something else for cleaner code
			index = 0;
			for (int j = 6; j < totalVerticesPerPoint; j += 3)
			{
				// Draw segment of triangle
				vertices[verticeIndex + j].set(originX, originY);
				vertices[verticeIndex + j + 1].set(originX + circlePoints[index].first, originY + circlePoints[index].second);
				vertices[verticeIndex + j + 2].set(originX + circlePoints[index + 1].first,
																					 originY + circlePoints[index + 1].second);

				index++;
			}

			// Return to x_i+1 in preparation for next iteration
			vertices[verticeIndex + totalVerticesPerPoint].set(originX, originY);
		}

		node->markDirty(QSGNode::DirtyGeometry);
	}

	return node;
}

//----------------------------------------------------------------------------------------------------------------------------------//
