# das2 file format specification

das2 is a binary runtime asset delivery format supported by [DENG](/deng/deng).
This documentation is mainly created for people implementing das2 into their projects.

## Concepts

### Matrices, quaternions and vectors

Matrices are represented in row-major order as two dimentional arrays. Supported matrix types are 2x2, 3x3 and 4x4.
Quaternions are represented as 4 element float arrays, similarly to vectors which are represented as N element arrays. 

### Scene and skeletal hierarchies

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

### Meshes, mesh groups and morph targets

Meshes essentially describe how vertices should be drawn. Mesh groups are then used to group together multiple meshes into one larger unit.
Each mesh can contain morph targets, which are used in morph animations. Meshes are described with `das2::Mesh`, mesh 
groups with `das2::MeshGroup` and morph targets with `das2::MorphTarget` structures accordingly.

Each mesh has an 8 element wide UV table, which is used to describe UV coordinates for different textures. The list
of table indices with corresponding texture type is described in a following list:
0. normal map (PBR and Phong)
1. occlusion map (PBR)
2. emission map (PBR and Phong)
3. albedo map (PBR)
4. metalness map (PBR)
5. roughness map (PBR)
6. phongDiffuse (Phong)
7. phongSpecular (Phong)

For more information about the material system in das2 format please read [this](#blinn-phong-and-pbr-materials).

### Multiple LODs

das2 can support multiple LODs of the same mesh in a single file. Multiple LODs can be utilized in scenes, where lots of meshes are used.
das2 library supports generating multiple LODs automatically as well as adding them manually, however due to the nature of [quadric error surface 
simplification algorithm](https://www.cs.cmu.edu/~./garland/Papers/quadrics.pdf) it is not yet possible to simplify meshes with skeleton 
joint attributes. 

Multiple LODs are stored inside `das2::Mesh` structure and implicitly the root mesh is **always** considered as `lod0`. All other LOD
levels must be greater than 1.

### Animations

Animations in das2 consist of channels, which describe keyframes, interpolation method and animated property values. Additionally, if
cubic spline interpolation is used, keyframe tangents are used in interpolation algorithm as well. Animations are defined as `
das2::Animation` structure and animation channels are defined as `das2::AnimationChannel`. There are the possible objects whose
properties can be animated, those are `das2::Node` and `das2::SkeletonJoint` described with `uNodePropertyId` and `uJointProperty`
variables accordingly. If one property is used then the other property **must** be set to -1.

### Materials

das2 supports two shading systems, namely Blinn-Phong shading and PBR or physics-based rendering approach which are described with `das2::MaterialPbr`
and `das2::MaterialPhong` accordingly. Both of these approaches can make use of different shading property maps which, when composed properly, can give
visually appealing results. Since das2 doesn't support embedded images, all textures must be described with external Uris. Conveniently in both 
`das2::MaterialPbr` and `das2::MaterialPhong` texture uri naming must end with `Uri` suffix.


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

| Data type     | Variable name                     | Description                                 | Default value | Modifiable |
|---------------|-----------------------------------|---------------------------------------------|---------------|------------|
| byte          | bStructure                        | structure identifier                        | x02           | no         |
| u32           | uIndexBufferId                    | ID of a buffer to use for indices data      | -1            | yes        |
| u32           | uIndexBufferOffset                | offset of the index buffer                  | 0             | yes        |
| u32           | uDrawCount                        | amount of vertices to draw                  | 0             | yes        |
| u32           | uPositionVertexBufferId           | ID of a buffer to use for position vertices | -1            | yes        |
| u32           | uPositionVertexBufferOffset       | offset of position buffer                   | 0             | yes        |
| u32           | uSurfaceNormalBufferId            | ID of a buffer to use for surface normals   | -1            | yes        |
| u32           | uSurfaceNormalBufferOffset        | offset of the surface normal buffer         | 0             | yes        |
| u32[8]        | uUVBufferIds                      | array of UV coordinate buffer ids           | [-1]          | yes        |
| u32[8]        | uUVBufferOffsets                  | array of UV coordinate buffer offsets       | [0]           | yes        |
| u32           | uColorMultiplierId                | ID of a buffer to use for color multipliers | -1            | yes        |
| u32           | uColorMultiplierOffset            | offset of the color multiplier buffer       | 0             | yes        |
| u32[8]        | uSkeletalJointIndexBufferIds      | array of skeletal joint index sets indices  | [-1]          | yes        |
| u32[8]        | uSkeletalJointIndexBufferOffsets  | array of skeletal joint index sets offsets  | [0]           | yes        |
| u32[8]        | uSkeletalJointWeightBufferIds     | array of skeletal joint weight sets indices | [-1]          | yes        |
| u32[8]        | uSkeletalJointWeightBufferOffsets | array of skeletal joint weight sets offsets | [0]           | yes        |
| byte          | bMaterialType                     | Material type descriptor                    | x00           | yes        |
| u32           | uMaterialId                       | ID of a material to use                     | -1            | yes        |
| u32           | uMorphTargetCount                 | number of morph targets per mesh            | 0             | yes        |
| [MorphTarget] | pMorphTargets                     | array of morph target structures            | []            | yes        |
| u32           | uMultipleLodCount                 | 

### das2::MorphTarget (body/x03)

#### Synopsis

Morph targets are used to define possible variations of the same mesh.

#### Structure

| Data type | Varible name                | Description                                 | Default value | Modifiable |
|-----------|-----------------------------|---------------------------------------------|---------------|------------|
| byte      | bStructure                  | structure identifier                        | x03           | no         |
| u32       | uIndexBufferId              | ID of a buffer to use for indices data      | -1            | yes        |
| u32       | uIndexBufferOffset          | offset of the index buffer                  | 0             | yes        |
| u32       | uPositionVertexBufferId     | ID of a buffer to use for position vertices | -1            | yes        |
| u32       | uPositionVertexBufferOffset | offset of the position vertex buffer        | 0             | yes        |
| u32       | uSurfaceNormalBufferId      | ID of a buffer to use for surface normals   | -1            | yes        |
| u32       | uSurfaceNormalBufferOffset  | offset of the surface normal buffer         | 0             | yes        |
| u32[8]    | uUVBufferIds                | array of UV coordinate buffer ids           | [-1]          | yes        |
| u32[8]    | uUVBufferOffsets            | array of UV coordinate buffer offsets       | [0]           | yes        |
| u32       | uColorMultiplierId          | ID of a buffer to use for color multipliers | -1            | yes        |
| u32       | uColorMultiplierOffset      | offset of the color multiplier buffer       | 0             | yes        |

### das2::MeshGroup (body/x04)

#### Synopsis

Mesh groups are used to logically group meshes into larger units that are drawn.

#### Structure

| Data type | Variable name | Description                        | Default value | Modifiable |
|-----------|---------------|------------------------------------|---------------|------------|
| byte      | bStructure    | structure identifier               | x03           | no         |
| String    | szName        | name of the mesh group             | nullstr       | yes        |
| u32       | uMeshCount    | number of meshes to group together | 0             | yes        |
| [u32]     | pMeshes       | array of mesh IDs to group         | []            | yes        |

### das2::Node (body/x05)

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

### das2::Scene (body/x06)

#### Synopsis

Scenes essentially define a hierarchy graph, using previously defined das2::Node structures.

#### Structure

| Data type | Variable name  | Description                       | Default value | Modifiable |
|-----------|----------------|-----------------------------------|---------------|------------|
| byte      | bStructure     | structure identifier              | x05           | no         |
| String    | szName         | name of the scene                 | nullstr       | yes        |
| u32       | uRootNodeCount | number of root nodes in a scene   | 0             | yes        |
| [u32]     | pRootNodes     | array of root node IDs in a scene | []            | yes        |

### das2::SkeletonJoint (body/x07)

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

### das2::Skeleton (body/x08)

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

### das2::Animation (body/x09)

#### Synopsis

das2::Animation structure is used to compose different animation channels.

#### Structure

| Data type | Variable name          | Description                    | Default value | Modifiable |
|-----------|------------------------|--------------------------------|---------------|------------|
| byte      | bStructure             | structure identifier           | x08           | no         |
| u32       | uAnimationChannelCount | animation channel count        | 0             | yes        |
| [u32]     | pAnimationChannels     | array of animation channel Ids | []            | yes        |

### das2::AnimationChannel (body/x0a)

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


### das2::MaterialPhong (body/x0b)

#### Synopsis

Phong material defines properties for Blinn-Phong shading model.

#### Structure 

| Data type | Variable name    | Description             | Default value  | Modifiable |
|-----------|------------------|------------------------ |----------------|------------|
| byte      | bStructure       | structure identifier    | x0a            | no         |
| String    | szName           | material name string    | nullstr        | yes        |
| Vector4f  | vDiffuse         | diffuse property        | [ 0, 0, 0, 1 ] | yes        |
| Vector4f  | vSpecular        | specular property       | [ 0, 0, 0, 1 ] | yes        |
| Vector4f  | vEmission        | light emission property | [ 0, 0, 0, 1 ] | yes        |
| String    | szDiffuseMapUri  | diffuse map uri         | nullstr        | yes        |
| String    | szSpecularMapUri | specular map uri        | nullstr        | yes        |
| String    | szEmissionMapUri | emission map uri        | nullstr        | yes        |

### das2::MaterialPbr (body/x0c)

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
