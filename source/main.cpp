#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QSurfaceFormat>

#include "Series/Series.h"

int main(int argc, char* argv[])
{
	QSurfaceFormat format;
	format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;

	qmlRegisterType<Series>("QFreeCharts.Series", 1.0, 0.0, "Series");

	const QUrl url(QStringLiteral("qrc:/Qml/Main.qml"));

	QObject::connect(
			&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
			Qt::QueuedConnection);

	engine.load(url);

	return app.exec();
}
