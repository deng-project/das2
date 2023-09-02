# das2 file format specification

das2 is a binary runtime asset delivery format supported by [DENG](/deng/deng).
This documentation is mainly created for people implementing das2 into their projects.

## Concepts

### Matrices, quaternions, vectors

### Scenes and hierarchies

das2 assets must contain at least one scene. Scenes are used to represent relations between different objects, which by themselves are bound to
scene nodes. In das2, scenes are defined with `das2::Scene` structure and nodes with `das2::Node` structure. Nodes can bind mesh groups to 
skeletons, give objects certain transformation properties and form an hierarchy that defines an order in which transformations need to be applied.
Let's assume, we have a following scene hierarchy
```
A 
 \
  B
 / \
C   D
```

The order of transformations in that particular hierarchy would look like this:
```
     A
     |
    A*B
   /   \
A*B*C A*B*D
```
Thus it is important to consider for implementations, that there is an implicit transformation dependency between all nodes in a particular tree.

Similar to scenes, hierarchies can be used in context of skeletons as well. Skeleton hierarchy doesn't use `das2::Node` structure but instead 
uses `das2::SkeletonJoint` structure. The main difference between `das2::Node` and `das2::JointNode` is that `das2::JointNode` contains 
inverse bind position matrix. This matrix, when applied, restores the origin position for that joint.


## Structure reference manual

das2 consists of two kinds of structures. The first structure `das2::Header` is used to store various 
kind of information about the file such as author's name, comment and license as well as information 
such as zlib compression level, vertices count etc. `das2::Header` itself is the only structure in the 
file that is not compressed at any zlib compression level. The other section of structures that are in das2
format are so-called body formats. These various formats declare buffers, meshes, skeletons, materials,
animations and so on.

### das2::Header (head/x0000000032736164) 

#### Synopsis 

The first structure that appears in the file, which contains various kind of information 
about the file itself.

#### Structure

| Data type | Variable name      | Description                                 | Default value     | Modifiable  |
|-----------|--------------------|---------------------------------------------|-------------------|-------------|
| u64       | uMagic             | das2 magic bytes                            | x0000000032736164 | no          |
| String    | szAuthorName       | author's name string                        | nullstr           | yes         |
| String    | szComment          | misc comment string                         | nullstr           | yes         |
| u32       | uVerticesCount     | cumulated number of vertices for all meshes | 0                 | yes         |
| u32       | uMeshCount         | number of meshes used in file               | 0                 | yes         |
| u32       | uAnimationCount    | number of animations used in file           | 0                 | yes         |
| u32       | uDefaultSceneIndex | index of the default scene to use           | 0                 | yes         |
| u8        | uZlibLevel         | zlib compression level [0-9]                | 0                 | yes         |

### das2::Buffer (body/x01)

#### Synopsis

Blob of data which contains various information about meshes itself.

#### Structure

| Data type | Variable name | Description          | Default value | Modifiable |
|-----------|---------------|----------------------|---------------|------------|
| byte      | bStructure    | structure identifier | x01           | no         |
| u32       | uLength       | length of the buffer | 0             | yes        |
| [byte]    | pData         | array of blob bytes  | []            | yes        |

NOTE: Since the integer width for describing the buffer length is 32 bits, the maximum buffer size is limited to 4GB.

### das2::Mesh (body/x02)

#### Synopsis

Mesh structure essentially describes the vertex attributes of a mesh and how it should be drawn.

#### Structure

| Data type | Variable name                     | Description                                    | Default value | Modifiable |
|-----------|-----------------------------------|------------------------------------------------|---------------|------------|
| byte      | bStructure                        | structure identifier                           | x02           | no         |
| u32       | uIndexBufferId                    | ID of a buffer to use for indices data         | -1            | yes        |
| u32       | uIndexBufferOffset                | offset of the index buffer                     | 0             | yes        |
| u32       | uDrawCount                        | amount of vertices to draw                     | 0             | yes        |
| u32       | uPositionVertexBufferId           | ID of a buffer to use for position vertices    | -1            | yes        |
| u32       | uPositionVertexBufferOffset       | offset of position buffer                      | 0             | yes        |
| u32       | uSurfaceNormalBufferId            | ID of a buffer to use for surface normals      | -1            | yes        |
| u32       | uSurfaceNormalBufferOffset        | offset of the surface normal buffer            | 0             | yes        |
| u32[8]    | uUVBufferIds                      | array of UV coordinate buffer ids              | [-1]          | yes        |
| u32[8]    | uUVBufferOffsets                  | array of UV coordinate buffer offsets          | [0]           | yes        |
| u32       | uColorMultiplierId                | ID of a buffer to use for color multiplication | -1            | yes        |
| u32       | uColorMultiplierOffset            | offset of the color multiplier                 | 0             | yes        |
| u32[8]    | uSkeletalJointIndexBufferIds      | array of skeletal joint index sets indices     | [-1]          | yes        |
| u32[8]    | uSkeletalJointIndexBufferOffsets  | array of skeletal joint index sets offsets     | [0]           | yes        |
| u32[8]    | uSkeletalJointWeightBufferIds     | array of skeletal joint weight sets indices    | [-1]          | yes        |
| u32[8]    | uSkeletalJointWeightBufferOffsets | array of skeletal joint weight sets offsets    | [0]           | yes        |
| byte      | bMaterialType                     | Material type descriptor                       | x00           | yes        |
| u32       | uMaterialId                       | ID of a material to use                        | -1            | yes        |

Order of UVs: normal, occlusion, emissive, albedo, metalness, roughness, phongDiffuse, phongSpecular

### das2::MeshGroup (body/x03)

#### Synopsis

Mesh groups are used to logically group meshes into larger units that are drawn.

#### Structure

| Data type | Variable name | Description                        | Default value | Modifiable |
|-----------|---------------|------------------------------------|---------------|------------|
| byte      | bStructure    | structure identifier               | x03           | no         |
| String    | szName        | name of the mesh group             | nullstr       | yes        |
| u32       | uMeshCount    | number of meshes to group together | 0             | yes        |
| [u32]     | pMeshes       | array of mesh IDs to group         | []            | yes        |

### das2::Node (body/x04)

#### Synopsis

Nodes are used as building blocks to define scene hierarchy.

#### Structure

| Data type  | Variable name    | Description                             | Default value     | Modifiable |
|------------|------------------|-----------------------------------------|-------------------|------------|
| byte       | bStructure       | structure identifier                    | x04               | no         |
| String     | szName           | name of the node                        | nullstr           | yes        |
| u32        | uChildrenCount   | number of children the node has         | 0                 | yes        |
| [u32]      | pChildren        | array of children the node has          | []                | yes        |
| u32        | uMeshGroupId     | ID of a mesh group to bind to that node | -1                | yes        |
| u32        | uSkeletonId      | ID of a skeleton to bind to that node   | -1                | yes        |
| Matrix4f   | mCustomTransform | custom transformation matrix            | *Identity matrix* | yes        |
| Quaternion | qRotation        | quaterion rotation                      | [ 0, 0, 0, 1 ]    | yes        |
| Vector3f   | vTranslation     | translation vector                      | [ 0, 0, 0 ]       | yes        |
| f32        | fScale           | scale transformation property           | 1.f               | yes        |

### das2::Scene (body/x05)

#### Synopsis

Scenes essentially define a hierarchy graph, using previously defined das2::Node structures.

#### Structure

| Data type | Variable name  | Description                       | Default value | Modifiable |
|-----------|----------------|-----------------------------------|---------------|------------|
| byte      | bStructure     | structure identifier              | x05           | no         |
| String    | szName         | name of the scene                 | nullstr       | yes        |
| u32       | uRootNodeCount | number of root nodes in a scene   | 0             | yes        |
| [u32]     | pRootNodes     | array of root node IDs in a scene | []            | yes        |

### das2::SkeletonJoint (body/x06)

#### Synopsis

Skeleton joints act as nodes for skeleton hierarchies

#### Structure

| Data type  | Variable name   | Description                  | Default value     | Modifiable |
|------------|-----------------|------------------------------|------------------ |------------|
| byte       | bStructure      | structure identifier         | x06               | no         |
| String     | szName          | name of the skeleton joint   | nullstr           | yes        |
| u32        | uChildrenCount  | number of children joints    | 0                 | yes        |
| [u32]      | pChildren       | array of child joints        | []                | yes        |
| Matrix4f   | mInverseBindPos | inverse bind position matrix | *Identity matrix* | yes        |
| Quaternion | qRotation       | rotation quaterion           | [ 0, 0, 0, 1 ]    | yes        |
| Vector3f   | vTranslation    | translation vector           | [ 0, 0, 0 ]       | yes        |
| f32        | fScale          | scale property               | 1.f               | yes        |

### das2::Skeleton (body/x07)

#### Synopsis

Skeletons are used to form skeletal hierarchies that can be used for skeletal animations. 

#### Structure

| Data type | Variable name | Description               | Default value | Modifiable |
|-----------|---------------|---------------------------|---------------|------------|
| byte      | bStructure    | structure identifier      | x07           | no         |
| String    | szName        | name of the skeleton      | nullstr       | yes        |
| u32       | uParent       | parent joint id           | -1            | yes        |
| u32       | uJointCount   | number of skeleton joints | 0             | yes        |
| [u32]     | uJoints       | array of skeleton joints  | []            | yes        |

### das2::Animation (body/x08)

#### Synopsis

das2::Animation structure is used to compose different animation channels.

#### Structure

| Data type | Variable name          | Description                    | Default value | Modifiable |
|-----------|------------------------|--------------------------------|---------------|------------|
| byte      | bStructure             | structure identifier           | x08           | no         |
| u32       | uAnimationChannelCount | animation channel count        | 0             | yes        |
| [u32]     | pAnimationChannels     | array of animation channel Ids | []            | yes        |

### das2::AnimationChannel (body/x09)

#### Synopsis

Animation channels contain information about animated properties on how the channel should be animated.

#### Structure

| Data type | Variable name      | Description                                                                  | Default value | Modifiable |
|-----------|--------------------|------------------------------------------------------------------------------|---------------|------------|
| byte      | bStructure         | structure identifier                                                         | x09           | no         |
| u32       | uNodePropertyId    | animated node property Id                                                    | -1            | yes        |
| u32       | uJointPropertyId   | animated joint property Id                                                   | -1            | yes        |
| byte      | bAnimationTarget   | animation target description byte                                            | x00           | yes        |
| byte      | bInterpolationType | animation interpolation type description byte                                | x00           | yes        |
| u32       | uKeyframeCount     | number of keyframes in the channel                                           | 0             | yes        |
| u32       | uWeightCount       | number of weight properties to animate                                       | 0             | yes        |
| [float]   | pKeyframes         | array of keyframes used in the channel                                       | []            | yes        |
| [byte]    | pTangents          | byte array buffer of keyframe tangents to use for cubic spline interpolation | []            | yes        |
| [byte]    | pTargetValues      | byte array of animated property target values                                | []            | yes        |


### das2::MaterialPhong (body/x0a)

#### Synopsis

Phong material defines properties for Blinn-Phong shading model.

#### Structure 

| Data type | Variable name | Description          | Default value  | Modifiable |
|-----------|---------------|----------------------|----------------|------------|
| byte      | bStructure    | structure identifier | x0a            | no         |
| String    | szName        | material name string | nullstr        | yes        |
| Vector4f  | vDiffuse      | diffuse property     | [ 0, 0, 0, 0 ] | yes        |
| Vector4f  | vSpecular     | specular property    | [ 0, 0, 0, 0 ] | yes        |
| String    | szDiffuseMap  | diffuse map uri      | nullstr        | yes        |
| String    | szSpecularMap | specular map uri     | nullstr        | yes        |

### das2::MaterialPbr (body/x0b)

#### Synopsis

PBR material structure defines properties for Physics Based Rendering model.

#### Structure

| Data type | Variable name            | Description                 | Default value  | Modifiable |
|-----------|--------------------------|-----------------------------|----------------|------------|
| byte      | bStructure               | structure identifier        | x0b            | no         |
| String    | szName                   | material name string        | nullstr        | yes        |
| Vector4f  | vAlbedoFactor            | albedo factor value         | [ 1, 1, 1, 1 ] | yes        |
| Vector4f  | vEmissiveFactor          | emissive factor value       | [ 0, 0, 0, 1 ] | yes        |
| f32       | fRoughness               | roughness parameter         | 0              | yes        |
| f32       | fMetallic                | metallicness parameter      | 0              | yes        |
| f32       | fAmbientOcclusion        | ambient occlusion parameter | 1              | yes        |
| String    | szAlbedoMapUri           | albedo map uri              | nullstr        | yes        |
| String    | szEmissionMapUri         | emission map uri            | nullstr        | yes        |
| String    | szRoughnessMap           | roughness map uri           | nullstr        | yes        |
| String    | szMetallicMap            | metallic map uri            | nullstr        | yes        |
| String    | szAmbientOcclusionMapUri | ambient occlusion map uri   | nullstr        | yes        |
