/**
 * @author Vladimir Chalupecky (vladimir.chalupecky@gmail.com)
 */

#include <map>
#include <vector>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "ExtractSurfacePlugin.hh"

ExtractSurfacePlugin::ExtractSurfacePlugin()
	: extractSurfaceMeshAction_()
{
}

void ExtractSurfacePlugin::initializePlugin()
{
}

void ExtractSurfacePlugin::pluginsInitialized()
{
	if (!OpenFlipper::Options::gui()) {
		return;
	}

	extractSurfaceMeshAction_ = new QAction(tr("Extract surface"), this);
	connect(extractSurfaceMeshAction_, SIGNAL(triggered()),
	        this, SLOT(slotExtractSurfaceMesh()));
	emit addContextMenuItem(extractSurfaceMeshAction_, DATA_POLYHEDRAL_MESH, CONTEXTOBJECTMENU);
}

void ExtractSurfacePlugin::slotUpdateContextMenu(int objectId)
{
}

void ExtractSurfacePlugin::slotExtractSurfaceMesh()
{
	int vmeshId = extractSurfaceMeshAction_->data().toInt();
	BaseObjectData *vmeshObject = NULL;
	if (!PluginFunctions::getObject(vmeshId, vmeshObject)) {
		return;
	}
	if (!vmeshObject->dataType(DATA_POLYHEDRAL_MESH)) {
		return;
	}

	PolyhedralMesh *vmesh = PluginFunctions::polyhedralMesh(vmeshObject);

	unsigned int maxFaceVertices = 0;
	for (OpenVolumeMesh::HalfFaceIter hfit = vmesh->halffaces_begin(); hfit != vmesh->halffaces_end(); ++hfit) {
		if (!vmesh->is_boundary(*hfit)) {
			continue;
		}
		unsigned int nFaceVertices = vmesh->halfface(*hfit).halfedges().size();
		if (nFaceVertices > maxFaceVertices) {
			maxFaceVertices = nFaceVertices;
		}
	}

	if (maxFaceVertices < 3) {
		emit log(LOGERR, tr("Polyhedral mesh %1 has no faces").arg(vmeshId));
		return;
	}

	// TODO: remove once implemented
	if (maxFaceVertices > 3) {
		emit log(LOGERR, tr("Surface extraction not yet implemented for polyhedral meshes with non-triangular surface"));
		return;
	}

	BaseObjectData *smeshObject = NULL;
	if (maxFaceVertices == 3) {
		int smeshId = RPC::callFunctionValue<int>("typetrianglemesh", "addEmpty");
		if (!PluginFunctions::getObject(smeshId, smeshObject)) {
			emit log(LOGERR, tr("Cannot get new triangle mesh object %1").arg(smeshId));
			return;
		}

		TriMesh* smesh = PluginFunctions::triMesh(smeshObject);
		extractSurface(*vmesh, *smesh);
	} else {
		int smeshId = RPC::callFunctionValue<int>("typepolymesh", "addEmpty");
		if (!PluginFunctions::getObject(smeshId, smeshObject)) {
			emit log(LOGERR, tr("Cannot get new polymesh object %1").arg(smeshId));
			return;
		}

		PolyMesh* smesh = PluginFunctions::polyMesh(smeshObject);
		extractSurface(*vmesh, *smesh);
	}
	smeshObject->setName("Surface of " + vmeshObject->name());
	smeshObject->setFileName(smeshObject->name());

	emit updatedObject(smeshObject->id(), UPDATE_ALL);
}

void ExtractSurfacePlugin::extractSurface(PolyhedralMesh const& vmesh, TriMesh &smesh)
{
	std::map<OpenVolumeMesh::VertexHandle, TriMesh::VertexHandle> svh;

	for (OpenVolumeMesh::VertexIter vit = vmesh.vertices_begin(); vit != vmesh.vertices_end(); ++vit) {
		if (!vmesh.is_boundary(*vit)) {
			continue;
		}
		Vector const& p = vmesh.vertex(*vit);
		svh[*vit] = smesh.add_vertex(p);
	}

	OpenVolumeMesh::VertexHandle bfv[3];
	for (OpenVolumeMesh::HalfFaceIter hfit = vmesh.halffaces_begin(); hfit != vmesh.halffaces_end(); ++hfit) {
		if (!vmesh.is_boundary(*hfit)) {
			continue;
		}

		OpenVolumeMesh::HalfFaceVertexIter vit = vmesh.hfv_iter(*hfit);
		bfv[0] = *vit++;
		bfv[1] = *vit++;
		bfv[2] = *vit;
		smesh.add_face(svh[bfv[0]], svh[bfv[1]], svh[bfv[2]]);
	}
	smesh.update_normals();
}

void ExtractSurfacePlugin::extractSurface(PolyhedralMesh const& vmesh, PolyMesh &smesh)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(extractsurfaceplugin, ExtractSurfacePlugin)
#endif
