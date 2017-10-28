#pragma once

#include <iostream>
#include <memory>
#include <string_view>

// #include <gsl/gsl>

#include <fbxsdk.h>
#include <fbxsdk\scene\geometry\fbxnode.h>

class FBX final {
public:

    explicit FBX();

    bool ImportScene(std::string_view path, std::string_view sceneName = "Scene");

    std::string GetMetaData() const;

    void DisplayHeirarchy(FbxNode const *node) const;
    void DisplayContent(FbxNode const *node) const;

    //void GetMetaDataConnections

private:

    template<class T>
    struct FBXObjectDeleter {
        void operator() (T *object)
        {
            object->Destroy();
        }
    };

    std::unique_ptr<FbxManager, FBXObjectDeleter<FbxManager>> manager_;
    std::unique_ptr<FbxIOSettings, FBXObjectDeleter<FbxIOSettings>> ios_;

    std::unique_ptr<FbxScene, FBXObjectDeleter<FbxScene>> scene_;

    void DisplayAttribute(FbxMesh const *mesh) const;
};

std::ostream &operator<< (std::ostream &stream, FbxNodeAttribute::EType attributeType);
std::ostream &operator<< (std::ostream &stream, FbxNodeAttribute const &attribute);
std::ostream &operator<< (std::ostream &stream, FbxDouble3 const &vector);
std::ostream &operator<< (std::ostream &stream, FbxAMatrix const &matrix);
std::ostream &operator<< (std::ostream &stream, FbxNode const &node);