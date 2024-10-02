#pragma once

#include "../Shaders/RotationMaterial.h"
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

	Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged);
	Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
	explicit Series(QQuickItem* parent = nullptr);
	~Series();

	Q_INVOKABLE void addPoint(qreal y);
	Q_INVOKABLE void replaceSeries(QVariantList points);

	QColor color() const;
	float lineWidth() const;

signals:
	void colorChanged();
	void lineWidthChanged();

public slots:
	void setColor(QColor color);
	void setLineWidth(float lineWidth);

protected:
	QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData);

private:
	QVariantList m_points;

	QColor m_color;
	float m_lineWidth;
};

//----------------------------------------------------------------------------------------------------------------------------------//
