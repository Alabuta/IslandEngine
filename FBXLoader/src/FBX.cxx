#include <sstream>

#include "FBX.h"

#if _DEBUG
#pragma comment(lib, "fbxsdk/debug/libfbxsdk-md.lib")
#elif NDEBUG
#pragma comment(lib, "fbxsdk/release/libfbxsdk-md.lib")
#endif

FBX::FBX() :
    manager_(FbxManager::Create()),
    ios_(FbxIOSettings::Create(manager_.get(), IOSROOT))
{
    int major, minor, revision;
    manager_->GetFileFormatVersion(major, minor, revision);

    std::cout << "FBX SDK version: " << major << '.' << minor << '.' << revision << ".\n";

    manager_->SetIOSettings(ios_.get());

    ios_->SetBoolProp(IMP_CAMERA, false);
    ios_->SetBoolProp(IMP_FBX_MATERIAL, true);
    ios_->SetBoolProp(IMP_FBX_TEXTURE, true);
    ios_->SetBoolProp(IMP_FBX_LINK, true);
    ios_->SetBoolProp(IMP_FBX_SHAPE, true);
    ios_->SetBoolProp(IMP_FBX_GOBO, false);
    ios_->SetBoolProp(IMP_FBX_ANIMATION, true);
    ios_->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

}

bool FBX::ImportScene(std::string_view _path, std::string_view _sceneName)
{
    scene_.reset(FbxScene::Create(manager_.get(), _sceneName.data()));

    {
        std::unique_ptr<FbxImporter, FBXObjectDeleter<FbxImporter>> importer(FbxImporter::Create(manager_.get(), ""));

        importer->Initialize(_path.data(), -1, ios_.get());

        if (!importer) {
            std::cout << "Call to FbxImporter::Initialize() failed.\n";
            std::cout << "Error returned: " << importer->GetStatus().GetErrorString() << ".\n";
            return false;
        }

        int major, minor, revision;
        importer->GetFileVersion(major, minor, revision);

        std::cout << "FBX file version: " << major << '.' << minor << '.' << revision << ".\n\n";

        importer->Import(scene_.get());

        std::cout << GetMetaData() << "\n\n";
    }

    auto const &globalSettings = scene_->GetGlobalSettings();

    if (globalSettings.GetSystemUnit() != FbxSystemUnit::m) {
        FbxSystemUnit::ConversionOptions constexpr conversionOptions = {
            false, // mConvertRrsNodes
            true, // mConvertAllLimits
            true, // mConvertClusters
            true, // mConvertLightIntensity
            true, // mConvertPhotometricLProperties
            true  // mConvertCameraClipPlanes
        };

        FbxSystemUnit::m.ConvertScene(scene_.get(), conversionOptions);
    }

    FbxAxisSystem::MayaYUp.ConvertScene(scene_.get());

    FbxNode *rootNode = scene_->GetRootNode();

   /* if (rootNode) {
        for (auto i = 0; i < rootNode->GetChildCount(); ++i)
            std::cout << *rootNode->GetChild(i);
    }*/

    DisplayHeirarchy(rootNode);
    DisplayContent(rootNode);

    return true;
}

std::string FBX::GetMetaData() const
{
    if (!scene_)
        return { };

    auto const info = scene_->GetSceneInfo();

    if (!info)
        return { };

    std::ostringstream ss;

    ss  << "Title: " << info->mTitle
        << "\nSubject: " << info->mSubject
        << "\nAuthor: " << info->mAuthor
        << "\nKeywords: " << info->mKeywords
        << "\nRevision: " << info->mRevision
        << "\nComment: " << info->mComment;

    return ss.good() ? ss.str() : "";
}

void FBX::DisplayHeirarchy(FbxNode const *node) const
{
    static size_t depth = 0;

    std::cout << std::string(depth, ' ') << node->GetName() << '\n';

    depth += 4;

    for (auto i = 0; i < node->GetChildCount(); ++i)
        DisplayHeirarchy(node->GetChild(i));

    depth -= 4;
}

void FBX::DisplayContent(FbxNode const *node) const
{
    auto attribute = node->GetNodeAttribute();

    if (!attribute)
        return;

    switch (attribute->GetAttributeType()) {
        case FbxNodeAttribute::eMesh:
            DisplayAttribute(dynamic_cast<FbxMesh const*>(attribute));
            break;

        default:
            break;
    }
}

void FBX::DisplayAttribute(FbxMesh const *mesh) const
{
    std::cout << mesh->GetName();

    ;
}

std::ostream &operator<< (std::ostream &stream, FbxNodeAttribute::EType attributeType)
{
    switch (attributeType) {
        case FbxNodeAttribute::eUnknown:
            stream << "undefined";
            break;

        case FbxNodeAttribute::eNull:
            stream << "null";
            break;

        case FbxNodeAttribute::eMarker:
            stream << "marker";
            break;

        case FbxNodeAttribute::eSkeleton:
            stream << "skeleton";
            break;

        case FbxNodeAttribute::eMesh:
            stream << "mesh";
            break;

        case FbxNodeAttribute::eNurbs:
            stream << "NURBS";
            break;

        case FbxNodeAttribute::ePatch:
            stream << "patch";
            break;

        case FbxNodeAttribute::eCamera:
            stream << "camera";
            break;

        case FbxNodeAttribute::eCameraStereo:
            stream << "stereo";
            break;

        case FbxNodeAttribute::eCameraSwitcher:
            stream << "camera switcher";
            break;

        case FbxNodeAttribute::eLight:
            stream << "light";
            break;

        case FbxNodeAttribute::eOpticalReference:
            stream << "optical reference";
            break;

        case FbxNodeAttribute::eOpticalMarker:
            stream << "optical marker";
            break;

        case FbxNodeAttribute::eNurbsCurve:
            stream << "NURBS curve";
            break;

        case FbxNodeAttribute::eTrimNurbsSurface:
            stream << "trim NURBS surface";
            break;

        case FbxNodeAttribute::eBoundary:
            stream << "boundary";
            break;

        case FbxNodeAttribute::eNurbsSurface:
            stream << "NURBS surface";
            break;

        case FbxNodeAttribute::eShape:
            stream << "shape";
            break;

        case FbxNodeAttribute::eLODGroup:
            stream << "LOD group";
            break;

        case FbxNodeAttribute::eSubDiv:
            stream << "subdiv";
            break;
    }

    return stream;
}

std::ostream &operator<< (std::ostream &stream, FbxNodeAttribute const &attribute)
{
    return stream << "\t<attribute type: " << attribute.GetAttributeType()
        << "; name: " << attribute.GetName() << ">\n";
}

std::ostream &operator<< (std::ostream &stream, FbxDouble3 const &vector)
{
    return stream << "\t" << vector[0] << ";\t" << vector[1] << ";\t" << vector[2];
}

std::ostream &operator<< (std::ostream &stream, FbxAMatrix const &matrix)
{
    stream << matrix.GetRow(0) << '\n';
    stream << matrix.GetRow(1) << '\n';
    stream << matrix.GetRow(2) << '\n';
    stream << matrix.GetRow(3);

    return stream;
}

std::ostream &operator<< (std::ostream &stream, FbxNode const &node)
{
    auto const nodeName = node.GetName();

    auto const translation = node.LclTranslation.Get();
    auto const rotation = node.LclRotation.Get();
    auto const scaling = node.LclScaling.Get();

    auto const &matrix = const_cast<FbxNode &>(node).EvaluateGlobalTransform();

    stream << "<node name: " << nodeName
        << "\n\ttranslation: " << translation
        << "\n\trotation: " << rotation
        << "\n\tscaling: " << scaling
        << "\n\tmatrix: " << matrix
        << ">\n";

    for (auto i = 0; i < node.GetNodeAttributeCount(); ++i)
        stream << *node.GetNodeAttributeByIndex(i);

    for (auto i = 0; i < node.GetChildCount(); ++i)
        std::cout << *node.GetChild(i);

    return stream << "</node>\n";
}