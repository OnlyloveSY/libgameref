//MIT License

//Copyright (c) 2021 OnlyloveSY

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

package main

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"strings"
)

const DefCount = 40

var NEXTCLRN = "\\" + CLRN

var CLRN = "\r\n"

func getLICENSE() string {
	return "//MIT License" + CLRN +
		"" + CLRN +
		"//Copyright (c) 2021 OnlyloveSY" + CLRN +
		"" + CLRN +
		"//Permission is hereby granted, free of charge, to any person obtaining a copy" + CLRN +
		"//of this software and associated documentation files (the \"Software\"), to deal" + CLRN +
		"//in the Software without restriction, including without limitation the rights" + CLRN +
		"//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" + CLRN +
		"//copies of the Software, and to permit persons to whom the Software is" + CLRN +
		"//furnished to do so, subject to the following conditions:" + CLRN +
		"" + CLRN +
		"//The above copyright notice and this permission notice shall be included in all" + CLRN +
		"//copies or substantial portions of the Software." + CLRN +
		"" + CLRN +
		"//THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" + CLRN +
		"//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," + CLRN +
		"//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" + CLRN +
		"//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" + CLRN +
		"//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," + CLRN +
		"//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" + CLRN +
		"//SOFTWARE."
}

func getBeginTitle() string {
	return "//===================================== Begin%d ============================================" + CLRN
}

func getHeadTitle() string {
	return "struct StructName;" + NEXTCLRN +
		"std::shared_ptr<StructValueConverter<StructName>> pref_##StructName##_config_ = std::make_shared<StructValueConverter<StructName>>(FilePath, delimiters);" + NEXTCLRN +
		"struct StructName {" + NEXTCLRN +
		"StructName():" + NEXTCLRN +
		"%s" + // add init()
		"{}" + NEXTCLRN
}

func getEndContent() string {
	return "~StructName() {}" + NEXTCLRN +
		"static StructValueConverter<StructName>* GetTablePtr(){" + NEXTCLRN +
		"StructValueConverter<StructName>* p =  pref_##StructName##_config_.get();" + NEXTCLRN +
		"if (p == nullptr) { return nullptr; } " + NEXTCLRN +
		"else {" + NEXTCLRN +
		"if(!p->IsEmpty()){return p;}" + NEXTCLRN +
		"else {" + NEXTCLRN +
		"%s" + //add Register
		"return p;" + NEXTCLRN +
		"}" + NEXTCLRN +
		"}" + NEXTCLRN +
		"}" + NEXTCLRN +
		"};"
}

const (
	T1 = "T%d_1"
	T2 = "T%d_2"
	T3 = "T%d_3"
)

//#define TABLE_STRUCT_DEF_%d(StructName,FilePath,delimiters,T1_1,T1_2,T1_3)\
func NewDefine(num int) string {
	str := ""
	for i := 1; i <= num; i++ {
		for j := 1; j <= 3; j++ {
			str += ","
			str += fmt.Sprintf("T%d_%d", i, j)
		}
	}
	return fmt.Sprintf("#define TABLE_STRUCT_DEF_%d(StructName,FilePath,delimiters%s)", num, str) + NEXTCLRN
}

func NewHead(num int) string {
	return fmt.Sprintf(getHeadTitle(), PackInit(num))
}

func PackRegister(num int) string {
	str := ""
	for i := 1; i <= num; i++ {
		str += fmt.Sprintf("p->Register(&StructName::T%d_2,T%d_3);", i, i) + NEXTCLRN
	}
	return str
}

func PackInit(num int) string {
	str := ""
	for i := 1; i <= num; i++ {
		if len(str) > 0 {
			str += ","
		}
		str += fmt.Sprintf("T%d_2(INIT(T%d_1))", i, i) + NEXTCLRN
	}
	return str
}

func NewVariable(num int) string {
	str := ""
	for i := 1; i <= num; i++ {
		str += fmt.Sprintf("T%d_1 T%d_2;%s", i, i, NEXTCLRN)
	}
	return str
}

func NewEndContent(num int) string {
	return fmt.Sprintf(getEndContent(), PackRegister(num))
}

func CreateAutoTableCode(num int) (ret strings.Builder) {
	ret.WriteString(NewDefine(num))
	ret.WriteString(NewHead(num))
	ret.WriteString(NewVariable(num))
	ret.WriteString(NewEndContent(num))
	return
}

func Generated() {
	sysType := runtime.GOOS

	if sysType == "linux" {
		CLRN = "\n"
	}

	if sysType == "windows" {
		CLRN = "\r\n"
	}

	f, err := os.OpenFile("./table_ref.h", os.O_RDWR|os.O_TRUNC|os.O_CREATE, 0766)
	if err != nil {
		log.Println(err.Error())
		return
	}

	_, err = f.WriteString(getLICENSE() + CLRN +
		"// Generated by the convert compiler.  DO NOT EDIT!" + CLRN +
		"#pragma once" + CLRN +
		"#include \"libref.hpp\"" + CLRN)

	for i := 1; i <= DefCount; i++ {
		str := CreateAutoTableCode(i)
		_, err = f.WriteString(CLRN + fmt.Sprintf(getBeginTitle(), i) + str.String() + CLRN)
	}
	defer f.Close()
}

func main() {
	Generated()
}
