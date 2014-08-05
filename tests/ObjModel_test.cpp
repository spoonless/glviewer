#include <gtest/gtest.h>

#include <sstream>
#include "ObjModel.hpp"

TEST(ObjModel, canLoadEmptyModel)
{
    vfm::ObjModel model;

    std::istringstream stream("");

    stream >> model;

    ASSERT_EQ(0u, model.vertices.size());
    ASSERT_EQ(0u, model.normals.size());
}

TEST(ObjModel, canLoadVertices)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only vertices\n"
        "v 0 0 0\n"
        "v 0.4 0.5 0.6\n"
        "v 1 1 1 0.5\n"
    );

    stream >> model;

    ASSERT_EQ(3u, model.vertices.size());
    ASSERT_EQ(glm::vec4(0,0,0,1), model.vertices[0]);
    ASSERT_EQ(glm::vec4(0.4,0.5,0.6,1), model.vertices[1]);
    ASSERT_EQ(glm::vec4(1,1,1,0.5), model.vertices[2]);
}

TEST(ObjModel, canLoadNormals)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only normals\n"
        "vn 0 0 0\n"
        "vn 0.4 0.5 0.6\n"
        "vn 1 1 1"
    );

    stream >> model;

    ASSERT_EQ(3u, model.normals.size());
    ASSERT_EQ(glm::vec3(0,0,0), model.normals[0]);
    ASSERT_EQ(glm::vec3(0.4,0.5,0.6), model.normals[1]);
    ASSERT_EQ(glm::vec3(1,1,1), model.normals[2]);
}

TEST(ObjModel, canLoadTextures)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only textures coord\n"
        "vt  1         \n"
        "vt 10   20    \n"
        "vt 100 200 300\n"
    );

    stream >> model;

    ASSERT_EQ(3u, model.textures.size());
    ASSERT_EQ(glm::vec3(1,0,0), model.textures[0]);
    ASSERT_EQ(glm::vec3(10,20,0), model.textures[1]);
    ASSERT_EQ(glm::vec3(100,200,300), model.textures[2]);
}

TEST(ObjModel, canHaveMultipleObjects)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only face indices\n"
        "f 1 2 3 4\n"
        "# only face indices\n"
        "o object2\n"
        "f 1 2 3 4 5\n"
        "o object3\n"
        "f 1 2 3 4 5 6\n"
    );

    stream >> model;

    ASSERT_EQ(3u, model.objects.size());

    ASSERT_EQ("", model.objects[0].name);
    ASSERT_EQ(4u, model.objects[0].vertexIndices.size());
    ASSERT_EQ("object2", model.objects[1].name);
    ASSERT_EQ(5u, model.objects[1].vertexIndices.size());
    ASSERT_EQ("object3", model.objects[2].name);
    ASSERT_EQ(6u, model.objects[2].vertexIndices.size());
}


TEST(ObjModel, canReadIndices)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only face indices\n"
        "f 1 2 3\n"
        "f 4/40 5/50 6/60\n"
        "f 4/40/400 5/50/500 6/60/600\n"
        "f 7//700 8//800 9//900"
    );

    stream >> model;

    ASSERT_EQ(1u, model.objects.size());

    vfm::VertexIndexVector &vertexIndices = model.objects[0].vertexIndices;

    ASSERT_EQ(12u, vertexIndices.size());

    ASSERT_EQ(vfm::VertexIndex(1, 0, 0), vertexIndices[0]);
    ASSERT_EQ(vfm::VertexIndex(2, 0, 0), vertexIndices[1]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[2]);

    ASSERT_EQ(vfm::VertexIndex(4, 0, 40), vertexIndices[3]);
    ASSERT_EQ(vfm::VertexIndex(5, 0, 50), vertexIndices[4]);
    ASSERT_EQ(vfm::VertexIndex(6, 0, 60), vertexIndices[5]);

    ASSERT_EQ(vfm::VertexIndex(4, 400, 40), vertexIndices[6]);
    ASSERT_EQ(vfm::VertexIndex(5, 500, 50), vertexIndices[7]);
    ASSERT_EQ(vfm::VertexIndex(6, 600, 60), vertexIndices[8]);

    ASSERT_EQ(vfm::VertexIndex(7, 700, 0), vertexIndices[9]);
    ASSERT_EQ(vfm::VertexIndex(8, 800, 0), vertexIndices[10]);
    ASSERT_EQ(vfm::VertexIndex(9, 900, 0), vertexIndices[11]);
}

TEST(ObjModel, canRemoveDuplicates)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only face indices\n"
        "f 4/40/400 5/50/500 6/60/600\n"
        "f 6/60/600 4/40/400 7/70/700\n"
    );

    stream >> model;

    ASSERT_EQ(1u, model.objects.size());

    vfm::VertexIndexVector &vertexIndices = model.objects[0].vertexIndices;

    ASSERT_EQ(4u, vertexIndices.size());

    ASSERT_EQ(vfm::VertexIndex(4, 400, 40), vertexIndices[0]);
    ASSERT_EQ(vfm::VertexIndex(5, 500, 50), vertexIndices[1]);
    ASSERT_EQ(vfm::VertexIndex(6, 600, 60), vertexIndices[2]);
    ASSERT_EQ(vfm::VertexIndex(7, 700, 70), vertexIndices[3]);
}

TEST(ObjModel, canTriangulate)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "# only face indices\n"
        "f 1 2 3\n"
        "f 1 2 3 4 5"
    );

    stream >> model;

    ASSERT_EQ(1u, model.objects.size());

    vfm::IndexVector &triangles = model.objects[0].triangles;
    vfm::VertexIndexVector &vertexIndices = model.objects[0].vertexIndices;

    ASSERT_EQ(4u * 3u, triangles.size());

    ASSERT_EQ(0u, triangles[0]);
    ASSERT_EQ(1u, triangles[1]);
    ASSERT_EQ(2u, triangles[2]);

    ASSERT_EQ(0u, triangles[3]);
    ASSERT_EQ(1u, triangles[4]);
    ASSERT_EQ(2u, triangles[5]);

    ASSERT_EQ(4u, triangles[6]);
    ASSERT_EQ(0u, triangles[7]);
    ASSERT_EQ(2u, triangles[8]);

    ASSERT_EQ(4u, triangles[9]);
    ASSERT_EQ(2u, triangles[10]);
    ASSERT_EQ(3u, triangles[11]);

    ASSERT_EQ(vfm::VertexIndex(1, 0, 0), vertexIndices[triangles[0]]);
    ASSERT_EQ(vfm::VertexIndex(2, 0, 0), vertexIndices[triangles[1]]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[triangles[2]]);

    ASSERT_EQ(vfm::VertexIndex(1, 0, 0), vertexIndices[triangles[3]]);
    ASSERT_EQ(vfm::VertexIndex(2, 0, 0), vertexIndices[triangles[4]]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[triangles[5]]);

    ASSERT_EQ(vfm::VertexIndex(5, 0, 0), vertexIndices[triangles[6]]);
    ASSERT_EQ(vfm::VertexIndex(1, 0, 0), vertexIndices[triangles[7]]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[triangles[8]]);

    ASSERT_EQ(vfm::VertexIndex(5, 0, 0), vertexIndices[triangles[9]]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[triangles[10]]);
    ASSERT_EQ(vfm::VertexIndex(4, 0, 0), vertexIndices[triangles[11]]);
}

TEST(ObjModel, canReadNegativeIndices)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "v 1 0 0\n"
        "v 2 0 0\n"
        "v 3 0 0\n"
        "f 1 2 3\n"
        "f -3 -2 -1\n"
    );

    stream >> model;

    ASSERT_EQ(1u, model.objects.size());

    vfm::VertexIndexVector &vertexIndices = model.objects[0].vertexIndices;
    vfm::IndexVector &triangles = model.objects[0].triangles;

    ASSERT_EQ(3u, vertexIndices.size());

    ASSERT_EQ(vfm::VertexIndex(1, 0, 0), vertexIndices[0]);
    ASSERT_EQ(vfm::VertexIndex(2, 0, 0), vertexIndices[1]);
    ASSERT_EQ(vfm::VertexIndex(3, 0, 0), vertexIndices[2]);

    ASSERT_EQ(0u, triangles[0]);
    ASSERT_EQ(1u, triangles[1]);
    ASSERT_EQ(2u, triangles[2]);

    ASSERT_EQ(0u, triangles[3]);
    ASSERT_EQ(1u, triangles[4]);
    ASSERT_EQ(2u, triangles[5]);
}

TEST(ObjModel, canLoadMaterial)
{
    vfm::ObjModel model;

    std::istringstream stream(
        "mtllib test\n"
        "f 1 2 3\n"
        "f 1 2 3\n"
        "usemtl t\n"
        "f 1 2 3\n"
        "usemtl t1\n"
        "f 1 2 3\n"
        "f 1 2 3\n"
        "f 1 2 3\n"
        "f 1 2 3\n"
        "f 1 2 3\n"
        "usemtl t\n"
        "f 1 2 3\n"
        "o another object\n"
        "f 1 2 3\n"
    );

    stream >> model;

    ASSERT_EQ(2u, model.objects.size());

    vfm::MaterialActivationVector *materialActivations = &model.objects[0].materialActivations;
    ASSERT_EQ(3u, materialActivations->size());

    vfm::MaterialActivation *materialActivation = &materialActivations->at(0);
    ASSERT_EQ("t", materialActivation->name);
    ASSERT_EQ(1u, materialActivation->materialLibrary);
    ASSERT_EQ(6u, materialActivation->start);
    ASSERT_EQ(9u, materialActivation->end);

    materialActivation = &materialActivations->at(1);
    ASSERT_EQ("t1", materialActivation->name);
    ASSERT_EQ(1u, materialActivation->materialLibrary);
    ASSERT_EQ(9u, materialActivation->start);
    ASSERT_EQ(24u, materialActivation->end);

    materialActivation = &materialActivations->at(2);
    ASSERT_EQ("t", materialActivation->name);
    ASSERT_EQ(1u, materialActivation->materialLibrary);
    ASSERT_EQ(24u, materialActivation->start);
    ASSERT_EQ(27u, materialActivation->end);

    materialActivations = &model.objects[1].materialActivations;
    ASSERT_EQ(1u, materialActivations->size());

    materialActivation = &materialActivations->at(0);
    ASSERT_EQ("t", materialActivation->name);
    ASSERT_EQ(1u, materialActivation->materialLibrary);
    ASSERT_EQ(0u, materialActivation->start);
    ASSERT_EQ(3u, materialActivation->end);
}
