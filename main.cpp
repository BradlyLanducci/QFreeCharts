#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QSurfaceFormat>

#include "FreeChart.h"

int main(int argc, char* argv[])
{
	// QSurfaceFormat format;
	// format.setSamples(4);
	// QSurfaceFormat::setDefaultFormat(format);

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	QObject::connect(
			&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
			Qt::QueuedConnection);

	qmlRegisterType<FreeChart>("FreeChart", 1.0, 0.0, "FreeChart");

	engine.loadFromModule("QFreeCharts", "Main");

	return app.exec();
}
