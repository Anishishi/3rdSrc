import sys
import re
import json

#open a given file
with open(sys.argv[1], "r", encoding="utf-8") as f:
    str1 = f.read()

#get author name
pattern1='<h1 class="author">(.*?)</h1>'
auth=re.findall(pattern1, str1, re.S)
dic1 = {}
dic1['author']=auth[0]

#get text
pattern2='<p pno=[0-9]+>(.*?)</p>'
results = re.findall(pattern2, str1, re.S)
result=''
for i in results:
    result+=i
str2=re.sub('<br>', '\n', result)
dic1['text']=str2

#dump dic1
with open('out.json', "w", encoding="utf-8") as f2:
    json.dump(dic1, f2)

