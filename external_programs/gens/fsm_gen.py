import re
import codecs
from collections import defaultdict

import sys

if __name__ == "__main__":
    if len(sys.argv) == 0:
        print(
            '''args: 
input=FILE1.gv
input_names=FILE2.l
output_h=FILE4.h
output_gv=FILE5.gv
class_name=NAME
            '''
        )
        exit(1)
    if (len(sys.argv) < 6):
         print ("мало аргументов")
         exit(1)
    input =         sys.argv[1]
    input_names =   sys.argv[2]
    output_h =      sys.argv[3]
    output_gv =     sys.argv[4]
    class_name =          sys.argv[5]
# input =     "D:\\repos\\BlackPhantomCoder\\lisp_core\\LispLibrary\\Input\\for_gens\\FSM_gen.gv"
# input_names =  "D:\\repos\\BlackPhantomCoder\\lisp_core\\LispLibrary\\Input\\for_gens\\FSM.l"
# output_h =  "D:\\repos\\BlackPhantomCoder\\lisp_core\\LispLibrary\\Input\\FSM\\FSM.h"
# output_gv = "D:\\repos\\BlackPhantomCoder\\lisp_core\\LispLibrary\\Input\\FSM\\FSM.gv"
# class_name = "FSM"

# получить ascii-код
def get_code(s):
    if(s == "EOB"):
        return -1
    pattern = r'(\\x[A-Za-z0-9]{2})' 
    xs = re.match(pattern, s)
    if not xs == None:
        return int(xs[1][2:],base=16)
    else:
        s = codecs.escape_decode(bytes(s, "utf-8"))[0].decode("utf-8")
        return ord(s[0])

# прочитать автомат из gv
def parse(lines, user_names):
    start = None
    sigma = defaultdict(list)
    v = {}
    for line in lines:
        line = line[:-1]
        match = re.search(r'([^-]+)->([^\[]+)(.+)?;', line)
        if(match != None):
            sourse = match[1].replace(" ", "").replace("\t", "")
            dest =  match[2].replace(" ", "").replace("\t", "")
            if(match[3] != None):
                params =  match[3].replace(" ", "").replace("\t", "")
                params_match = re.search(r'label=\"(([^\"\\]|\\\\|\\\"|\\[^\"])*)\"' , params)
                if not params_match == None:
                    s = params_match[1]
                    s = re.sub(r'(\\(\\[^$\\-]+))', "\\2", s, count=0)
                    
                    
                    match = re.match("(.+)-(.+)", s)
                    #print(s,match)
                    if(match != None):
                        #print(f"{get_code(match[1])} - {get_code(match[2])}")
                        #print([get_code(match[1]), get_code(match[2])])
                        sigma[sourse].append({"dest": dest, "cond": [get_code(match[1]), get_code(match[2])]})
                    else:
                        sigma[sourse].append({"dest": dest, "cond": [get_code(s)]})
                        #print(f"{get_code(s)}")
                    #print(f"{line} to {sourse},{dest},{params}")
            else:
                if(sourse == "init"):
                    #print(dest, "start")
                    start = dest
        else:
            match = re.search(r'([^\[\t ]+)[\t ]*\[label=\"([^\"]*)\"(,peripheries=2)?\]', line)
            if(match != None):
                sourse = match[1].replace(" ", "").replace("\t", "")
                name = match[2].replace("[", "").replace("]", "")
                int_name = int(name) if name != "" else -1
                # if(name in user_names):
                #     v[sourse] = {"name" : user_names[name], "terminal" :match[3] != None}
                # else:
                v[sourse] = {"name" : int_name, "terminal" :match[3] != None}
                #print( v[sourse])
    i = 1
    v2 = {}
    sigma2 =  defaultdict(list)
    names = {}
    for key1, val1 in v.items():
        name = key1
        if name == start:
            v2["s0"] = val1
            names[name] = "s0"
        else:
            v2[f"s{i}"] = val1
            names[name] = f"s{i}"
            i = i + 1
    for key, val in sigma.items():
        for t in val:
            sigma2[names[key]].append({"dest": names[t["dest"]], "cond": t["cond"]})
    v = v2
    sigma = sigma2
    start = "s0"

    print(start)
    print(sigma)
    print(v)
    return start, sigma, v

# сгенерировать
def gen_gv(start, sigma, v, user_names):
    with open(output_gv, mode="w") as f:
        f.write(
        '''digraph INITIAL {
		rankdir=LR;
		concentrate=true;
		node [fontname="ArialNarrow"];
		edge [fontname="Courier"];

		init [root=true,peripheries=0,label="INITIAL",fontname="Courier"];
		init ->''' + f"{start};\n")
        for key, val in v.items():
            name = val["name"]
            terminal = val["terminal"]
            user_name = user_names[name]
            user_name = "" if user_name == "none" else user_name
            f.write(f"{key} [label=\"{user_name}\"" + (",peripheries=2" if terminal else "") +"]\n")
            if(key in sigma.keys()):
                for t in sigma[key]:
                    cond = t["cond"]
                    dest = t["dest"]
                    if(len(cond) == 1 and cond[0] == -1):
                        f.write(f"{key} -> {dest} [label=\"EOF\",style=\"dashed\"]\n")
                    else:
                        f.write(f"{key} -> {dest} [label=\"{cond}\"]\n")
            #else: 
                # print(f"with: {key}")
        f.write("}")

# сгенерировать хедер файл с классом автомата
def gen_code(start, sigma, v, user_names, class_name):
    with open(output_h, mode="w") as f:
        at_end = ""
        next = ""
        for key, vals in sigma.items():
            s = ""
            s += f"        case {key}:\n"
            for val in vals:
                dest = val["dest"]
                if(len(val["cond"]) == 1):
                    code = val["cond"][0]
                    if(code == -1):
                        at_end += f"        case {key}:\n"
                        at_end += "             {\n"
                        at_end += f"                 t_state = {dest};\n"
                        at_end += f"                 return true;\n"
                        at_end += "             }\n"
                    else:
                        s += f"            if(ch == {code}) \n"
                else:
                    lcode = val["cond"][0]
                    rcode = val["cond"][1]
                    s += f"            if(ch >= {lcode} && ch <= {rcode})\n"
                s += "             {\n"
                s += f"                 t_state = {dest};\n"
                s += f"                 return true;\n"
                s += "             }\n"
            s += f"         break;\n"
            next += s
        terminal = ""
        names = ""
        for key, val in v.items():
            if(val["terminal"]):
                s = ""
                s += f"        case {key}:\n"
                terminal += s
                name = val["name"]
                s = ""
                s += f"        case {key}:\n"
                s += f"              return T_id::{user_names[name]};\n"
                names += s
        if(terminal != ""):
            terminal += "              return true;\n"
        f.write(
'''#include <string>
'''
f'''class {class_name}
'''
'''{
    enum states {
'''
f'''        {",".join(v.keys())}
'''
'''     };
public:
    enum class T_id {
'''
f'''        {",".join([name for key, name in user_names.items()])}
'''
'''     };
public:
'''
f'''    {class_name}() = default;
'''
'''
    void restart(){
'''
f'''        t_state =  {start};'''
'''
    }
    //EOF symbol (if needed)
    bool at_end(){
'''
+
(
'''
        switch(t_state){
'''
        +at_end +
'''        default:
            break;
        }
'''
if at_end != "" else ''''''
)
+
'''
        return false;
    }
    // reject/no
    bool next(unsigned char ch){
'''
+
(
'''
        switch(t_state){
'''
        +next +
'''        default:
            break;
        }
'''
if next != "" else ''''''
)
+
'''
        return false;
    }
    // is in Terminal
    bool is_T() const{
'''
+
(
'''
        switch(t_state){
'''
        +terminal +
'''        default:
            break;
        }
'''
if terminal != "" else ''''''
)
+
'''
        return false;
    }
    // -1 for non T
    T_id get_T_id() const{
'''
+
(
'''
        switch(t_state){
'''
        +names +
'''        default:
            break;
        }
'''
if names != "" else ''''''
)
+
'''
        return  T_id::none;
    }
    
private:
'''
f'''    states t_state = {start};
'''
'''};'''
        )

# прочитать имена конечных вершин (//#{name})
def parse_names(lines):
    names = {}
    for line in lines:
        line = line[:-1]
        match = re.search(r'//#[\t ]*([^\t ]+)[\t ]*', line)
        if(match != None):
            names[len(names)+1] = match[1]
    return names
                    

# программа
with open(input_names, encoding="utf-8") as f:
    user_names = parse_names(f.readlines())
    user_names [-1] = "none"

with open(input, encoding="utf-8") as f:
    start, sigma, v = parse(f.readlines(), user_names)
    gen_gv(start, sigma, v, user_names)
    gen_code(start, sigma, v, user_names, class_name)