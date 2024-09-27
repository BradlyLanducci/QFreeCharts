#include "FreeChart.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
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
				double frequencyX = 0.1;	// Adjust this for the X frequency
				double frequencyY = 0.15; // Adjust this for the Y frequency
				double amplitudeX = 1.0;	// Adjust this for the X amplitude
				double amplitudeY = 1.0;	// Adjust this for the Y amplitude
				double dphase = M_PI / 4; // Phase difference between X and Y

				// Generate points for a Lissajous curve
				for (qsizetype i = 0; i < 50; i++)
				{
					double t = i * frequencyX;									 // Time or parameter
					double x = amplitudeX * sin(2 * t + dphase); // X-coordinate
					double y = amplitudeY * sin(3 * t);					 // Y-coordinate
					points.append((y + 1.0) / 2.0);							 // Append the point
				}

				// Update the phase for the next update (to create movement)
				phase += 0.01; // Increment the phase to shift the sine wave over tim

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
	const int numCircleSegments = 10;
	const int numCircleVertices = numCircleSegments * 3;
	const int numRectangleVertices = 6;

	const int totalVerticesPerPoint = numCircleVertices + numRectangleVertices;

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

		float xWidth = width() / (pointsSize - 1);

		if (QOpenGLContext::currentContext())
		{
			QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
			functions->glEnable(GL_BLEND);
			functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard blending mode
			functions->glEnable(GL_MULTISAMPLE);
		}

		for (int i = 0; i < pointsSize - 1; i++)
		{
			double x1 = i * xWidth;
			double y1 = m_points[i].toDouble() * height();

			double x2 = (i + 1) * xWidth;
			double y2 = m_points[i + 1].toFloat() * height();

			// Series line thickness
			double lineWidth = 10.0;

			double diagnol = std::sqrt(std::pow(x2 - x1, 2.0) + std::pow(y2 - y1, 2.0));

			// Half of rectangles "height"
			double hypotenuse = lineWidth / 2.0;
			// Angle of rotation from xi to xi+1
			double theta = std::acos((x2 - x1) / diagnol);

			// Adjacent
			double s1 = hypotenuse * std::sin(theta);
			// Opposite
			double s2 = hypotenuse * std::cos(theta);

			int verticeIndex = i * totalVerticesPerPoint;

			// s1 = std::clamp(s1, 1.0, s1);
			// s2 = std::clamp(s2, 1.0, s2);

			// s1 = lineWidth;
			// s2 = lineWidth;

			// qDebug() << "s1" << s1 << "s2" << s2;

			// Top left
			double v1x = std::round(x1 - s1);
			double v1y = std::round(y1 + s2);
			// Bottom left
			double v2x = std::round(x1 + s1);
			double v2y = std::round(y1 - s2);
			// Top right
			double v3x = std::round(x2 - s1);
			double v3y = std::round(y2 + s2);
			// Bottom right
			double v4x = std::round(x2 + s1);
			double v4y = std::round(y2 - s2);

			// vertices[verticeIndex].set(v3x, v3y);
			// vertices[verticeIndex + 1].set(v4x, v4y);
			// vertices[verticeIndex + 2].set(v1x, v1y);
			// vertices[verticeIndex + 3].set(v4x, v4y);
			// vertices[verticeIndex + 4].set(v2x, v2y);
			// vertices[verticeIndex + 5].set(v1x, v1y);

			vertices[verticeIndex].set(v1x, v1y);
			vertices[verticeIndex + 1].set(v2x, v2y);
			vertices[verticeIndex + 2].set(v3x, v3y);
			vertices[verticeIndex + 3].set(v2x, v2y);
			vertices[verticeIndex + 4].set(v4x, v4y);
			vertices[verticeIndex + 5].set(v3x, v3y);

			// set back to origin to draw circle
			vertices[verticeIndex + 6].set(x2, y2);
		}

		node->markDirty(QSGNode::DirtyGeometry);
	}

	return node;
}

//----------------------------------------------------------------------------------------------------------------------------------//
