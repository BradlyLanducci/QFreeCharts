#pragma once

#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickItem>
#include <QTimer>
#include <random>

//----------------------------------------------------------------------------------------------------------------------------------//

class Series : public QQuickItem
{
	Q_OBJECT
public:
	explicit Series(QQuickItem* parent = nullptr);
	~Series();

	Q_INVOKABLE void addPoint(qreal y);
	Q_INVOKABLE void replaceSeries(QVariantList points);

signals:

public slots:

protected:
	QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData);

private:
	float phase{0.f};
	QVariantList m_points;

	QTimer* mp_timer;

	std::random_device rd;
	std::mt19937 gen{rd()}; // Mersenne Twister engine
	std::uniform_real_distribution<float> dis{0.0, 1.0};
};

//----------------------------------------------------------------------------------------------------------------------------------//
