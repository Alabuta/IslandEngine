/****************************************************************************************
****
****    Source code of Island Engine.
****	Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: Autodesk FBX file loading routines.
****
****************************************************************************************/
#if _DEBUG
#define _CRUS_MEMORY_CONTROL        1   // Use this to check for memory leaks!
#endif

#if _CRUS_MEMORY_CONTROL
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#endif // _CRUS_MEMORY_CONTROL

#include <iostream>
#include <memory>
#include <string>
//#include <string_view>

#include <fbxsdk.h>
#include <fbxsdk\scene\geometry\fbxnode.h>

//#pragma comment(linker, "libfbxsdk.lib")
//#pragma comment(linker, "libfbxsdk-mt.lib")
#if _DEBUG
#pragma comment(lib, "fbxsdk/debug/libfbxsdk-md.lib")
#elif NDEBUG
#pragma comment(lib, "fbxsdk/release/libfbxsdk-md.lib")
#endif


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
    auto attribName = attribute.GetName();

    return stream << "\t<attribute type: " << attribute.GetAttributeType()
        << "; name: " << attribName << ">\n";
}

std::ostream &operator<< (std::ostream &stream, FbxDouble3 const &vector)
{
    return stream << "\t" << vector[0] << ";\t" << vector[1] << ";\t" << vector[2];
}

std::ostream &operator<< (std::ostream &stream, FbxNode const &node)
{
    auto const nodeName = node.GetName();

    auto const translation = node.LclTranslation.Get();
    auto const rotation = node.LclRotation.Get();
    auto const scaling = node.LclScaling.Get();

    stream << "<node name: " << nodeName
        << "\n\ttranslation: " << translation
        << "\n\trotation: " << rotation
        << "\n\tscaling: " << scaling
        << ">\n";

    for (auto i = 0; i < node.GetNodeAttributeCount(); ++i)
        stream << *node.GetNodeAttributeByIndex(i);

    for (auto i = 0; i < node.GetChildCount(); ++i)
        std::cout << *node.GetChild(i);

    return stream << "</node>\n";
}

int main()
{
#if _CRUS_MEMORY_CONTROL
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    //_CrtSetBreakAlloc(84);
#endif

    std::string const file_name = R"(..\contents\meshes\cube.fbx)";

    auto fbxManagerDeleter = [] (FbxManager *fbx)
    {
        fbx->Destroy();
    };

    std::unique_ptr<FbxManager, decltype(fbxManagerDeleter)> fbxManager(FbxManager::Create(), fbxManagerDeleter);

    auto fbxIOSettingsDeleter = [] (FbxIOSettings *io)
    {
        io->Destroy();
    };

    std::unique_ptr<FbxIOSettings, decltype(fbxIOSettingsDeleter)> ios(FbxIOSettings::Create(fbxManager.get(), IOSROOT), fbxIOSettingsDeleter);
    fbxManager->SetIOSettings(ios.get());

    auto fbxSceneDeleter = [] (FbxScene *scene)
    {
        scene->Destroy();
    };

    std::unique_ptr<FbxScene, decltype(fbxSceneDeleter)> fbxScene(FbxScene::Create(fbxManager.get(), "myScene"), fbxSceneDeleter);

    {
        auto fbxImporterDeleter = [] (FbxImporter *importer)
        {
            importer->Destroy();
        };

        std::unique_ptr<FbxImporter, decltype(fbxImporterDeleter)> fbxImporter(FbxImporter::Create(fbxManager.get(), ""), fbxImporterDeleter);

        fbxImporter->Initialize(file_name.data(), -1, fbxManager->GetIOSettings());

        if (!fbxImporter) {
            std::cout << "Call to FbxImporter::Initialize() failed.\n";
            std::cout << "Error returned: " << fbxImporter->GetStatus().GetErrorString() << '.\n\n';
        }

        fbxImporter->Import(fbxScene.get());
    }

    auto rootNodeDeleter = [] (FbxNode *root)
    {
        root->Destroy();
    };

    std::unique_ptr<FbxNode, decltype(rootNodeDeleter)> rootNode(fbxScene->GetRootNode(), rootNodeDeleter);

    if (rootNode) {
        for (auto i = 0; i < rootNode->GetChildCount(); ++i)
            std::cout << *rootNode->GetChild(i);
    }

    std::cout << "\nDone..." << std::endl;
    std::cin.get();
    return 0;
}