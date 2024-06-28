[Font Tables - TrueType Reference Manual - Apple Developer](https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html)

## 二阶贝塞尔曲线

$$ {二阶贝塞尔曲线}
p(t) = (1-t)^2p0 + 2t(1-t)p1 + t^2p2
$$

$$ {二阶贝塞尔曲线,步进版}
p(STEP_{当前}) = \frac{(STEP_总-STEP_{当前})^2p0 + 2STEP_{当前}(STEP_总-STEP_{当前})p1 + STEP_{当前}^2p2}{STEP_总}\\
\Delta{P())}=P(STEP_{当前}+1)-p(STEP_{当前}) =\frac{(-2p0+2p1)}{STEP_总}+\frac{2STEP_{当前}}{STEP_总^2}(p0-2p1+p2)+\frac{1}{STEP_总^2}(p0-2p1+p2)\\
\Delta{\Delta{P()}} = \Delta{P(STEP_{当前})} - \Delta{P(STEP_{当前}-1)} = \frac{2}{STEP_总^2}(p0-2p1+p2)
$$

## ![image-20240619145759378](C:\Users\admin\AppData\Roaming\Typora\typora-user-images\image-20240619145759378.png)



## TTF表目录

| 参数                    | 含义                                                         | 大小          |
| ----------------------- | ------------------------------------------------------------ | ------------- |
| version                 | 版本号,用于指示用于栅格化此字体的 OFA 缩放器;有关详细信息<br/>`0x74727565`:`true`OS X 和 iOS的字体 <br/>`0x00010000`:Windows 或 Adobe 产品的字体<br/>`0x74797031`:`typ1`引用包含在 sfnt 包装器中的旧样式的 PostScript 字体<br/>`0x4F54544F`:`OTTO`带有 PostScript 轮廓的 OpenType 字体<br/>   `CFF ` `glyf`其他值不支持; | 4             |
| tables_num              | 表数目                                                       | 2             |
| search_range            | 二分法查找参数,小于或等于表中项目数的 2 的最大幂，即可以轻松搜索的最大项目数。 | 2             |
| entry_selector          | 二分法查找参数,在搜索循环开始之前，将目标项目与数字范围Shift的项目进行比较。如果目标项小于 rangeShift，则从表的开头进行搜索。如果更大，请从带有 number rangeShift 的项目开始搜索。 | 2             |
| range_shift             | 二分法查找参数,搜索循环需要多少次迭代                        | 2             |
| table_entry[tables_num] | 表入口                                                       | 16*tables_num |
| ~~table~~               | ~~表数据~~                                                   | ~~X~~         |



## TTF表入口格式

| 参数      | 含义         | 大小 |
| --------- | ------------ | ---- |
| tag       | 表名(表标签) | 4    |
| check_sum | 校验和       | 4    |
| addr      | 表入口地址   | 4    |
| length    | 表长度       | 4    |



## 必须表
<!----- 仅对 TrueType 字体是必需的。其他种类的 sfnt 字体可能没有它们 ----->

| 表名(表标签) | 描述                                                         |
| ------------ | ------------------------------------------------------------ |
| `cmap`       | 多种字符编码对应到字体形状的映射表                           |
| `glyf`       | 包含每个字体的形状数据                                       |
| `head`       | 字体头部，包含一些设置参数,字体的全局信息                    |
| `hhea`       | horizontal header(不知道怎么翻译，避免歧义直接用原文档的术语) |
| `hmtx`       | 字体水平布局星系：上高、下高、行间距、最大前进宽度、最小左支撑、最小右支撑 |
| `loca`       | 记录每个字体形状存在于文件内的哪个offset上                   |
| `maxp`       | 记录字体对于内存上的一些需求参数,字体中所需内存分配情况的汇总数据 |
| `name`       | 版权说明、字体名、字体族名、风格名等等                       |
| `post`       | 所有图元的PostScript  FontInfo目录项和PostScript名           |



## 可选表

| 表名(表标签) | 描述 |
| -------- | ---- |
| `acnt` | accent attachment |
| `ankr` | anchor point |
| `avar` | axis variation |
| `bdat` | bitmap data |
| `bhed` | 无轮廓位图字体头,结构与head表相同 |
| `bloc` | bitmap location |
| `bsln` | baseline |
| `cvar` | character code mapping |
| `cvt` | CVT variation |
| `EBSC` | embedded bitmap scaling control |
| `fdsc` | font descriptor |
| `feat` | layout feature |
| `fmtx` | font metrics |
| `fond` | font family compatibility |
| `fpgm` | font program |
| `fvar` | font variation |
| `gasp` | grid-fitting and scan-conversion procedure |
| `glyf` | glyph outline |
| `gvar` | glyph variation |
| `hdmx` | horizontal device metrics |
| `just` | justification |
| `kern` | 字距调整表 |
| `kerx` | extended kerning |
| `lcar` | ligature caret |
| `ltag` | language tag |
| `maxp` | maximum profile |
| `meta` | metadata |
| `mort` | (metamorphosis) table (deprecated) |
| `morx` | extended metamorphosis |
| `opbd` | optical bounds |
| `OS/2` | compatibility |
| `prep` | control value program |
| `prop` | properties |
| `sbix` | extended bitmaps |
| `trak` | tracking |
| `vhea` | vertical header |
| `vmtx` | vertical metrics |
| `xref` | cross-reference |
| `Zapf` | glyph reference |



## 必选表`head`/`bhed`

<!----- 没有轮廓数据但仅由位图组成的 TrueType 字体不应具有表。他们应该改用逐字节相同的“bhed”表。 ----->

| 名称                 | 类型         | 描述                                                         |
| -------------------- | ------------ | ------------------------------------------------------------ |
| version              | Fixed        | 0x00010000代表version1.0                                     |
| font_revision        | Fixed        | 格式同version,由字体制造商设置                               |
| check_sum_adjustment | uint32_t     |                                                              |
| magic_number         | uint32_t     | 0x5f0f3cf5                                                   |
| flags                | uint16_t     | 位 0 - y 值为 0 指定基线<br/>位 1 - 左侧的 x 位置 最黑的位是 LSB<br/>位 2 - 缩放的点大小和实际点大小会有所不同（即 24 点字形与按系数 2 缩放的 12 点字形不同）<br/>位 3 - 使用整数缩放而不是小数<br/>位 4 - 由 TrueType 缩放器的 Microsoft 实现使用<br/>位 5 - 此位应设置为垂直布局的字体，并且已绘制字形，使 x 坐标 0 对应于所需的垂直基线。<br/>位 6 - 此位必须设置为零。<br/>位 7 - 如果字体需要布局以实现正确的语言呈现（例如阿拉伯语字体），则应设置此位。<br/>位 8 - 此位应设置为 AAT 字体，该字体具有一个或多个默认指定的变形效果。<br/>位 9 - 如果字体包含任何从右到左的强字形，则应设置此位。<br/>bit 10 - 如果字体包含印度语样式的重新排列效果，则应设置此位。<br/>位 11-13 - 由 [Adobe](http://partners.adobe.com/asn/developer/opentype/head.htm) 定义。<br/>位 14 - 如果字体中的字形只是码位范围的通用符号，例如最后的字体，则应设置此位。 |
| unit_per_em          | uint16_t     | 范围从 64 到 16384                                           |
| created              | longDataTime | 1904/1/1 0:00开始计算的秒数,创建时间                         |
| modified             | longDataTime | 1904/1/1 0:00开始计算的秒数,修改时间                         |
| x_min                | FWord        | 对于所有字形边界框                                           |
| y_min                | FWord        | 对于所有字形边界框                                           |
| x_max                | FWord        | 对于所有字形边界框                                           |
| y_max                | FWord        | 对于所有字形边界框                                           |
| mac_style            | uint16_t     | 位 0 粗体<br/>位 1 斜体<br/>位 2 下划线<br/>位 3 轮廓<br/>位 4 阴影<br/>位 5 压缩（窄）<br/>位 6 扩展 |
| lowest_rec_ppem      | uint16_t     | 最小可读尺寸（以像素为单位）                                 |
| font_dir_hint        | int16_t      | 0 混合方向字形<br/>1 仅从左到右的字形<br/>2 类似 1 但也包含中性<br/>-1 仅从右到左的字形<br/>-2 类似-1但也包含中性 |
| index2loc_fmt        | int16_t      | 0 表示短偏移量，1 表示长偏移量                               |
| glyph_fmt            | int16_t      | 0 表示当前格式                                               |



## 必选表`maxp`

| 名称                   | 类型     | 描述                                       |
| ---------------------- | -------- | ------------------------------------------ |
| Version                | Fixed    |                                            |
| num_Glyphs             | uint16_t | 字体中的字形数                             |
| max_Points             | uint16_t | 非复合字形中的点数                         |
| max_Contours           | uint16_t | 非复合字形中的轮廓                         |
| max_CompositePoints    | uint16_t | 复合字形中的点                             |
| max_CompositeContours  | uint16_t | 复合字形中的轮廓                           |
| max_Zones              | uint16_t | 2                                          |
| max_TwilightPoints     | uint16_t |                                            |
| max_Storage            | uint16_t | 存储区域位置的数量                         |
| max_FunctionDefs       | uint16_t | FDEF 数量                                  |
| maxInstructionDefs     | uint16_t | IDEF 数量                                  |
| max_StackElements      | uint16_t | 最大堆深度                                 |
| max_SizeOfInstructions | uint16_t | 字形指令的字节计数                         |
| max_ComponentElements  | uint16_t | 顶层引用的字形数                           |
| max_ComponentDepth     | uint16_t | 递归级别，如果字体只有简单字形，则设置为 0 |



## 必须表`cmap`

### CMAP索引

| 名称                    | 类型     | 描述           |
| ----------------------- | -------- | -------------- |
| version                 | uint16_t | 版本号         |
| subtables_num           | uint16_t | 编码子表的数量 |
| subtable[subtables_num] |          | 编码子表       |

### CMAP编码子表

| 名称             | 类型     | 描述                   |
| ---------------- | -------- | ---------------------- |
| platform_id      | uint16_t | 平台标识符(Unicode 0)  |
| platform_spec_id | uint16_t | 特定于平台的编码标识符 |
| offset           | uint32_t | 映射表的偏移量         |

#### platform_id

| Platform ID | Platform           | Platform-specific ID                                         |
| ----------- | ------------------ | ------------------------------------------------------------ |
| 0           | Unicode            | 指示 Unicode 版本<br>0	版本 1.0 （已弃用）<br>1	版本 1.1 （已弃用）<br>2	ISO 10646 1993（已弃用）<br>3	Unicode 2.0 或更高版本（仅限 BMP）子表格式4 or 6<br>4	Unicode 2.0 或更高版本（允许使用非 BMP 字符,Unicode全表）子表格式10 or 12<br>5	Unicode变体序列-用于子表格式14<br>6    Unicode全表-用于子表格式13 |
| 1           | Macintosh          | 脚本管理器代码                                               |
| 2           | *ISO [deprecated]* | ISO编码(弃用)<br/>0	7-bit ASCII<br/>1	ISO 10646<br/>2	ISO 8859-1（已弃用） |
| 3           | Microsoft          | Microsoft编码<br/>0	Symbol<br/>1	仅限 Unicode BMP （UCS-2）<br/>2	Shift-JIS<br/>3	PRC<br/>4	BigFive<br/>5	Wansung<br/>6    Johab<br>7    reserved<br>8    reserved<br/>9    reserved<br/>10	Unicode UCS-4 |
| 4           | Custom             | Custom                                                       |
| other       | unknow(忽略CMAP表) |                                                              |

### CAMP 格式

每个`“cmap”`子表都位于当前九个子表之一中 可用格式。它们是FMT0、FMT2、FMT4、FMT6、 FMT8、FMT10、FMT12、FMT13 和FMT14 

FMT0支持Macintosh标准字符到字形的映射。

FMT2支持8/16位混合映射，对日语、中文和韩语很有用。

FMT4用于16位映射。

FMT6用于密集的16位映射。

FMT8,10,12,13和14用于16/32位和纯32位混合映射。这支持Unicode 2.0及更高版本中使用代理编码的文本。

许多cmap格式要么是过时的，要么是为了满足从未实现的预期需求而设计的。现代字体生成工具可能不需要能够以FMT4和12以外的格式编写通用的cmap。FMT13和14都是专用的。FMT13在结构上与FMT12相同(但对数据的解释不同)，因此对它的支持(如果需要)相对容易提供。使用Unicode变体选择器需要支持FMT14编码子表。

### FMT0
Format 0适用于字符码和字形索引限制为单个字节的字体。当TrueType被引入时，这是一个非常常见的情况，但现在很少遇到了。
| 名称            | 类型     | 描述                                            |
| --------------- | -------- | ----------------------------------------------- |
| format          | uint16_t | 0                                               |
| length          | uint16_t | 子表的长度（以字节为单位）（格式 0 设置为 262） |
| language        | uint16_t | 语言代码                                        |
| glyphIndex[256] | uint8_t  | 将字符代码映射到字形索引值的数组                |

### FMT2

| 名称                 | 类型        | 描述                                             |
| -------------------- | ----------- | ------------------------------------------------ |
| format               | uint16_t    | 2                                                |
| length               | uint16_t    | 子表的长度（以字节为单位）                       |
| language             | uint16_t    | 语言代码                                         |
| subHeaderKeys[256]   | uint16_t    | 将高字节映射到 subHeaders 的数组：值为 index * 8 |
| subHeaders[variable] | subheader_t | subHeader 结构的可变长度数组                     |
| glyphIndex[variable] | uint16_t    | 将字符代码映射到字形索引值的数组                 |

```c
typedef struct{
	uint16_t	firstCode;
	uint16_t	entryCount;
	int16_t		idDelta;
	uint16_t	idRangeOffset;
}subheader_t;
```

### FMT4

| 名称                       | 类型     | 描述                                      |
| -------------------------- | -------- | ----------------------------------------- |
| format                     | uint16_t | 4                                         |
| length                     | uint16_t | 子表的长度（以字节为单位）                |
| language                   | uint16_t | 语言代码                                  |
| segcount_x2                | uint16_t | 2*segcount                                |
| search_range               | uint16_t |                                           |
| entry_selector             | uint16_t |                                           |
| range_shift                | uint16_t |                                           |
| end_code[seg_count]        | uint16_t | 每个段的结束字符代码，last = 0xFFFF       |
| reserved                   | uint16_t | 0                                         |
| start_code[seg_count]      | uint16_t | 每个段的起始字符代码                      |
| id_delta[seg_count]        | int16_t  | 段中所有字符代码的增量                    |
| id_range_offset[seg_count] | uint16_t | 以字节为单位的glyph indexArray偏移量，或0 |
| glyph_index[variable]      | uint16_t | 将字符代码映射到字形索引值的数组          |

### FMT6



### FMT8



### FMT10



### FMT12



### FMT13



### FMT14





## 必须表`glyf`

### 通用描述

| 名称         | 类型    | 描述                                 |
| ------------ | ------- | ------------------------------------ |
| contours_num | int16_t | 轮廓数量;正数:简单图元,负数:复合图元 |
| x_min        | FWord   | 对于所有字形边界框                   |
| y_min        | FWord   | 对于所有字形边界框                   |
| x_max        | FWord   | 对于所有字形边界框                   |
| y_max        | FWord   | 对于所有字形边界框                   |

### 普通glyf定义

| 名称                            | 类型             | 描述                                                         |
| ------------------------------- | ---------------- | ------------------------------------------------------------ |
| end_points[contours_num]        | uint16_t         | 每个轮廓线的最后一个点的数组;contours_num 是轮廓线的数量;数组条目是点索引 |
| instruction_length              | uint16_t         | 指令所需的总字节数                                           |
| instructions[instructionLength] | uint8_t          | 此字形的指令数组                                             |
| flags[val]                      | uint8_t          | 标志数组                                                     |
| x_coordinates[]                 | uint8_t or int16 | x 坐标数组;第一个是相对于 (0,0),其他是相对于前一个点         |
| y_coordinates[]                 | uint8_t or int16 | y 坐标数组;第一个是相对于 (0,0),其他是相对于前一个点         |

#### flag

| BIT       | BIT7     | BIT6     | BIT5                                                         | BIT4                                                         | BIT3                                   | BIT2                      | BIT1                      | BIT0                           |
| --------- | -------- | -------- | ------------------------------------------------------------ | ------------------------------------------------------------ | -------------------------------------- | ------------------------- | ------------------------- | ------------------------------ |
| **FLAG**  | Reserved | Reserved | This y is same (Positive y-Short vector)                     | This x is same (Positive x-Short vector)                     | Repeat                                 | y-Short Vector            | x-Short Vector            | On Curve                       |
| **TRUE**  | /        | /        | 如果设置了 y-Short Vector 位，则该位描述值的符号，值 1 等于正值，零值等于负值。<br/>如果未设置 y-Short Vector，并且设置了此位，则当前 y 坐标与之前的 y 坐标相同。 | 如果设置了 x-Short Vector 位，则该位描述值的符号，值 1 等于正值，零值等于负值。<br/>如果未设置 x-short Vector 位，并且设置了此位，则当前 x 坐标与之前的 x 坐标相同。 | 下一个字节指定要重复此组标志的额外次数 | 对应y 坐标长度为 1 个字节 | 对应x 坐标长度为 1 个字节 | 点在轮廓上                     |
| **FALSE** | 设置为零 | 设置为零 | 如果未设置 y-Short Vector，并且未设置此位，则当前 y 坐标是有符号的 16 位增量向量。 | 如果未设置 x-short 向量位，并且未设置此位，则当前 x 坐标是有符号的 16 位增量向量。 |                                        | 对应y 坐标长度为 2 个字节 | 对应x 坐标长度为 2 个字节 | 点不在轮廓上(贝塞尔曲线控制点) |



### 复合glyf定义

| 名称                      | 类型                            | 描述                                     |
| ------------------------- | ------------------------------- | ---------------------------------------- |
| flags                     | uint16_t                        | 组件标志                                 |
| index                     | uint16_t                        | Glyph 组件目录                           |
| arg1                      | int16_t/uint16_t/int8_t/uint8_t | 类型取决于组件标志,代表组件X偏移量或点数 |
| arg2                      | int16_t/uint16_t/int8_t/uint8_t | 类型取决于组件标志,代表组件Y偏移量或点数 |
| **transformation option** |                                 |                                          |

#### flag

| BIT       | 15   | 14   | 13   | 12                        | 11                      | 10                   | 9                            | 8                              | 7                       | 6                         | 5                              | 4    | 3                    | 2             | 1           | 0         |
| --------- | ---- | ---- | ---- | ------------------------- | ----------------------- | -------------------- | ---------------------------- | ------------------------------ | ----------------------- | ------------------------- | ------------------------------ | ---- | -------------------- | ------------- | ----------- | --------- |
| **FLAG**  | /    | /    | /    | UNSCALED_COMPONENT_OFFSET | SCALED_COMPONENT_OFFSET | compound             | metrics                      | instruct                       | two_by_two              | xy_scale                  | more_comps                     | /    | scale                | arg_gird      | arg_val     | arg_type  |
| **TRUE**  | /    | /    | /    | /                         | /                       | 复合字形的组件将重叠 | 将此组件中的指标用于复合字形 | 组件字符的指令遵循最后一个组件 | 使用 2×2 转换来缩放组件 | x 方向与 y 方向不同的刻度 | 字形后面至少会有一个额外的字形 | /    | 组件有一个普通的比例 | arg值栅格化   | arg为偏移量 | arg为16位 |
| **FALSE** | 0    | 0    | 0    | /                         | /                       |                      |                              |                                |                         |                           | 结束                           | 0    | 比例为 1.0           | arg值非栅格化 | arg为点数   | arg为8位  |

#### transformation option

| option                  | Meaning                               |
| ----------------------- | ------------------------------------- |
| transformation entry #1 | 比例（x 和 y 相同）                   |
| transformation entry #2 | X刻度<br/>Y刻度                       |
| transformation entry #3 | X刻度<br/>刻度01<br/>刻度10<br/>Y刻度 |

#### 组件线性变换

| scale | xy_scale | two_by_two | a         | b         | c         | d         |
| ----- | -------- | ---------- | --------- | --------- | --------- | --------- |
| 0     | 0        | 0          | 1         | 0         | 0         | 1         |
| 1     | 0        | 0          | 1st short | 0         | 0         | 1st short |
| 0     | 1        | 0          | 1st short | 0         | 0         | 2nd short |
| 0     | 0        | 1          | 1st short | 2nd short | 3rd short | 4th short |

| arg_type | arg_val | e                                                            | f                                                        |
| -------- | ------- | ------------------------------------------------------------ | -------------------------------------------------------- |
| 0        | 0       | 1st byte containing index of matching point in compound being constructed | 2nd byte containing index of matching point in component |
| 0        | 1       | 1st byte contains the value of e                             | 2nd byte contains the value of f                         |
| 1        | 0       | 1st short contains the index of matching point in compound being constructed | 2nd short contains index of matching point in component  |
| 1        | 1       | 1st short contains the value of e                            | 2nd short contains the value of f                        |

$$
x'=m((\frac{a}{m})x+(\frac{c}{m})y+e)\\
y'=m((\frac{b}{m})x+(\frac{d}{m})y+f)
$$

m 和 n 的计算公式如下：

首先，设 m₀ = max（|a|， |b|） 和 n₀ = max（|c|， |d|）。

如果 |（|a|-|c|）|≤33/65536，则 m = 2m₀;否则，m = m₀。

同样，如果 |（|b|-|d|）|≤33/65536，则 n = 2n₀;否则，n = n₀



