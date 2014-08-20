#pragma once

/**
 * @author Vladimir Chalupecky (vladimir.chalupecky@gmail.com)
 */

#include <set>
#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

class ExtractSurfacePlugin : public QObject,
	BaseInterface,
	ContextMenuInterface,
	LoadSaveInterface,
	LoggingInterface,
	RPCInterface
{
	Q_OBJECT
	Q_INTERFACES(BaseInterface)
	Q_INTERFACES(ContextMenuInterface)
	Q_INTERFACES(LoadSaveInterface)
	Q_INTERFACES(LoggingInterface)
	Q_INTERFACES(RPCInterface)

#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-ExtractSurface")
#endif

public:
	ExtractSurfacePlugin();

signals:
	// BaseInterface
	void updateView();
	void updatedObject(int objectId, UpdateType const& type);

	// ContextMenuInterface
	void addContextMenuItem(QAction *action, ContextMenuType type);
	void addContextMenuItem(QAction *action, DataType objectType, ContextMenuType type);

	// LoggingInterface
	void log(Logtype type, QString message);
	void log(QString message);

private slots:
	// BaseInterface
	void initializePlugin();
	void pluginsInitialized();

	// Context menu handling
	void slotUpdateContextMenu(int objectId);

private slots:
	void slotExtractSurfaceMesh();

private:
	void extractSurface(PolyhedralMesh const& vmesh, TriMesh &smesh);
	void extractSurface(PolyhedralMesh const& vmesh, PolyMesh &smesh);

	QAction *extractSurfaceMeshAction_;

public:
	QString name()
	{
		return QString("ExtractSurfacePlugin");
	}

	QString description()
	{
		return QString("Extract the surface of a polyhedral mesh");
	}

public slots:
	QString version()
	{
		return QString("1.0");
	}
};
