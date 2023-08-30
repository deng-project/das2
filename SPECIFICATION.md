# das2 file format specification

das2 is a binary runtime asset delivery format supported by [DENG](/deng/deng).
This documentation is mainly created for people implementing das2 into their projects.

## Conventions

* Structure is a block of meaningful data that can be serialized or unserialized
* Each structure has a specific signature byte representing it
* String representation in binary is done like this: `{ u8: encoding, u16: uStringlength, [char]: data }`.
  This structure is represented as `String` data type later. Keep in mind that `uStringLength` property doesn't include trailing `\x00` character.

## Reference manual

das2 consists of two kinds of structures. The first structure `DasHeader` is used to store various 
kind of information about the file such as author's name, comment and license as well as information 
such as zlib compression level, vertices count etc. `DasHeader` itself is the only structure in the 
file that is not compressed at any zlib compression level. The other section of structures that are in das2
format are so-called body formats. These various formats declare buffers, meshes, skeletons, materials,
animations and so on.

### DasHeader (head/x00) 

#### Synopsis 

The first structure that appears in the file, which contains various kind of information 
about the file itself.

#### Structure

| Data type | Variable name      | Description                                 | Default value     | Modifiable  |
|-----------|--------------------|---------------------------------------------|-------------------|-------------|
| byte      | bStructure         | structure identifier                        | x00               | no          |
| u64       | uMagic             | das2 magic bytes                            | x0000000032736164 | no          |
| String    | szAuthorName       | author's name string                        | null              | yes         |
| String    | szComment          | misc comment string                         | null              | yes         |
| u32       | uVerticesCount     | cumulated number of vertices for all meshes | 0                 | yes         |
| u32       | uMeshCount         | number of meshes used in file               | 0                 | yes         |
| u32       | uAnimationCount    | number of animations used in file           | 0                 | yes         |
| u32       | uDefaultSceneIndex | index of the default scene to use           | 0                 | yes         |
| u8        | uZlibLevel         | zlib compression level [0-9]                | 0                 | yes         |

### DasBuffer (body/x01)

#### Synopsis

Blob of data which contains various information about the mesh itself.

#### Structure

| Data type | Variable name | Description          | Default value | Modifiable |
|-----------|---------------|----------------------|---------------|------------|
| byte      | bStructure    | structure identifier | x01           | no         |
| u32       | uLength       | length of the buffer | 0             | yes        |
| [byte]    | pData         | array of blob bytes  | []            | yes        |

NOTE: Since the integer width for describing the buffer length is 32 bits, the maximum buffer size is limited to 4GB.

### DasMesh (body/x02)

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

Order of UVs: normal, occlusion, emissive, albedo, metalness, roughness, phongDiffuse, phongSpecular

### DasMeshGroup (body/x03)

#### Synopsis

Mesh groups are used to logically group meshes into larger units that are drawn.


#### Structure

| Data type | Variable name | Description                        | Default value | Modifiable |
|-----------|---------------|------------------------------------|---------------|------------|
| byte      | bStructure    | structure identifier               | x03           | no         |
| u32       | uMeshCount    | number of meshes to group together | 0             | yes        |
| [u32]     | pMeshes       | array of mesh IDs to group         | []            | yes        |
