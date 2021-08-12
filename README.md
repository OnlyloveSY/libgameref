# 1.Guide

EN:Head Only C ++ - based game server or client is an efficient table reading tool, Reflect table data into the structure
CN:跨平台适用于游戏服务器,自动将以\t进行分割的数据表高效反射到对应标记的结构体成员中,他仅仅包含下面两个文件,并且没有其他任何依赖,具体如何使用请查看test文件夹中的测试用例,其中包含大量数据的表作为耗时参考

- See the test case in (test) for details on how to use it

- If you want to use it, just include the following files

- `libref.hpp`

- `table_ref.h`

  

# 2.API function docs

- To create a table structure object with a structure as its template, you pass in the path to the table and the split character

```c++
static StructValueConverter<StructType>* CreateTable(const char* file_name, const char d = '\t');
```



- Register a store of the underlying type

```c++
void Register(FieldType StructType::* field_pointer,std::string bind_field);
```



- Register a store for std::vector,

```c++
void Register(std::vector<FieldType> StructType::* field_pointer,std::string bind_field);
```



- Register a store for std::set/unordered_set,

```c++
void Register(std::unordered_set<FieldType> StructType::* field_pointer,std::string bind_field);
```



- Export as  STL map/unordered_map  data structure

```c++
bool Unmarshal(std::unordered_map<KeyType, StructType>& save);
```



- Export as  STL map/unordered_map  data structure,and support for lambda to check data easily

```c++
bool Unmarshal(std::unordered_map<KeyType, StructType>& save, _Pr pr);
```



- Export as  STL vector  data structure

```c++
bool Unmarshal(std::vector<StructType>& save);
```



- Export as  STL vector  data structure,and support for lambda to check data easily


```c++
bool Unmarshal(std::vector<StructType>& save, _Pr pr);
```





# 3.test

### Support

| variable type               | *test support* |
| :-------------------------- | -------------- |
| int8_t                      | &#10004;       |
| uint8_t                     | &#10004;       |
| int16_t                     | &#10004;       |
| uint16_t                    | &#10004;       |
| int32_t                     | &#10004;       |
| uint32_t                    | &#10004;       |
| int64_t                     | &#10004;       |
| uint64_t                    | &#10004;       |
| std::string                 | &#10004;       |
| std::set/std::unordered_set | &#10004;       |
| std::vector                 | &#10004;       |

### Unmarshal Support

| c++ stl            | *test support* |
| ------------------ | -------------- |
| std::vector        | &#10004;       |
| std::unordered_map | &#10004;       |
| std::map           | &#10004;       |



# 4.Generated Game Config Tool

If you want to expand or trim some fields, you can use the GneratedTool tool

```
go build -o tools
./tools
or
go run tools
```

