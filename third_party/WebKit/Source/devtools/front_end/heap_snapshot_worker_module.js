console=console;console.__originalAssert=console.assert;console.assert=function(value,message)
{if(value)
return;console.__originalAssert(value,message);}
var ArrayLike;Object.isEmpty=function(obj)
{for(var i in obj)
return false;return true;}
Object.values=function(obj)
{var result=Object.keys(obj);var length=result.length;for(var i=0;i<length;++i)
result[i]=obj[result[i]];return result;}
function mod(m,n)
{return((m%n)+n)%n;}
String.prototype.findAll=function(string)
{var matches=[];var i=this.indexOf(string);while(i!==-1){matches.push(i);i=this.indexOf(string,i+string.length);}
return matches;}
String.prototype.lineEndings=function()
{if(!this._lineEndings){this._lineEndings=this.findAll("\n");this._lineEndings.push(this.length);}
return this._lineEndings;}
String.prototype.lineCount=function()
{var lineEndings=this.lineEndings();return lineEndings.length;}
String.prototype.lineAt=function(lineNumber)
{var lineEndings=this.lineEndings();var lineStart=lineNumber>0?lineEndings[lineNumber-1]+1:0;var lineEnd=lineEndings[lineNumber];var lineContent=this.substring(lineStart,lineEnd);if(lineContent.length>0&&lineContent.charAt(lineContent.length-1)==="\r")
lineContent=lineContent.substring(0,lineContent.length-1);return lineContent;}
String.prototype.escapeCharacters=function(chars)
{var foundChar=false;for(var i=0;i<chars.length;++i){if(this.indexOf(chars.charAt(i))!==-1){foundChar=true;break;}}
if(!foundChar)
return String(this);var result="";for(var i=0;i<this.length;++i){if(chars.indexOf(this.charAt(i))!==-1)
result+="\\";result+=this.charAt(i);}
return result;}
String.regexSpecialCharacters=function()
{return"^[]{}()\\.^$*+?|-,";}
String.prototype.escapeForRegExp=function()
{return this.escapeCharacters(String.regexSpecialCharacters());}
String.prototype.escapeHTML=function()
{return this.replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;");}
String.prototype.unescapeHTML=function()
{return this.replace(/&lt;/g,"<").replace(/&gt;/g,">").replace(/&#58;/g,":").replace(/&quot;/g,"\"").replace(/&#60;/g,"<").replace(/&#62;/g,">").replace(/&amp;/g,"&");}
String.prototype.collapseWhitespace=function()
{return this.replace(/[\s\xA0]+/g," ");}
String.prototype.trimMiddle=function(maxLength)
{if(this.length<=maxLength)
return String(this);var leftHalf=maxLength>>1;var rightHalf=maxLength-leftHalf-1;return this.substr(0,leftHalf)+"\u2026"+this.substr(this.length-rightHalf,rightHalf);}
String.prototype.trimEnd=function(maxLength)
{if(this.length<=maxLength)
return String(this);return this.substr(0,maxLength-1)+"\u2026";}
String.prototype.trimURL=function(baseURLDomain)
{var result=this.replace(/^(https|http|file):\/\//i,"");if(baseURLDomain){if(result.toLowerCase().startsWith(baseURLDomain.toLowerCase()))
result=result.substr(baseURLDomain.length);}
return result;}
String.prototype.toTitleCase=function()
{return this.substring(0,1).toUpperCase()+this.substring(1);}
String.prototype.compareTo=function(other)
{if(this>other)
return 1;if(this<other)
return-1;return 0;}
function sanitizeHref(href)
{return href&&href.trim().toLowerCase().startsWith("javascript:")?null:href;}
String.prototype.removeURLFragment=function()
{var fragmentIndex=this.indexOf("#");if(fragmentIndex==-1)
fragmentIndex=this.length;return this.substring(0,fragmentIndex);}
String.hashCode=function(string)
{if(!string)
return 0;var result=0;for(var i=0;i<string.length;++i)
result=(result*3+string.charCodeAt(i))|0;return result;}
String.isDigitAt=function(string,index)
{var c=string.charCodeAt(index);return 48<=c&&c<=57;}
String.prototype.toBase64=function()
{function encodeBits(b)
{return b<26?b+65:b<52?b+71:b<62?b-4:b===62?43:b===63?47:65;}
var encoder=new TextEncoder();var data=encoder.encode(this.toString());var n=data.length;var encoded="";if(n===0)
return encoded;var shift;var v=0;for(var i=0;i<n;i++){shift=i%3;v|=data[i]<<(16>>>shift&24);if(shift===2){encoded+=String.fromCharCode(encodeBits(v>>>18&63),encodeBits(v>>>12&63),encodeBits(v>>>6&63),encodeBits(v&63));v=0;}}
if(shift===0)
encoded+=String.fromCharCode(encodeBits(v>>>18&63),encodeBits(v>>>12&63),61,61);else if(shift===1)
encoded+=String.fromCharCode(encodeBits(v>>>18&63),encodeBits(v>>>12&63),encodeBits(v>>>6&63),61);return encoded;}
String.naturalOrderComparator=function(a,b)
{var chunk=/^\d+|^\D+/;var chunka,chunkb,anum,bnum;while(1){if(a){if(!b)
return 1;}else{if(b)
return-1;else
return 0;}
chunka=a.match(chunk)[0];chunkb=b.match(chunk)[0];anum=!isNaN(chunka);bnum=!isNaN(chunkb);if(anum&&!bnum)
return-1;if(bnum&&!anum)
return 1;if(anum&&bnum){var diff=chunka-chunkb;if(diff)
return diff;if(chunka.length!==chunkb.length){if(!+chunka&&!+chunkb)
return chunka.length-chunkb.length;else
return chunkb.length-chunka.length;}}else if(chunka!==chunkb)
return(chunka<chunkb)?-1:1;a=a.substring(chunka.length);b=b.substring(chunkb.length);}}
Number.constrain=function(num,min,max)
{if(num<min)
num=min;else if(num>max)
num=max;return num;}
Number.gcd=function(a,b)
{if(b===0)
return a;else
return Number.gcd(b,a%b);}
Number.toFixedIfFloating=function(value)
{if(!value||isNaN(value))
return value;var number=Number(value);return number%1?number.toFixed(3):String(number);}
Date.prototype.toISO8601Compact=function()
{function leadZero(x)
{return(x>9?"":"0")+x;}
return this.getFullYear()+
leadZero(this.getMonth()+1)+
leadZero(this.getDate())+"T"+
leadZero(this.getHours())+
leadZero(this.getMinutes())+
leadZero(this.getSeconds());}
Date.prototype.toConsoleTime=function()
{function leadZero2(x)
{return(x>9?"":"0")+x;}
function leadZero3(x)
{return"0".repeat(3-x.toString().length)+x;}
return this.getFullYear()+"-"+
leadZero2(this.getMonth()+1)+"-"+
leadZero2(this.getDate())+" "+
leadZero2(this.getHours())+":"+
leadZero2(this.getMinutes())+":"+
leadZero2(this.getSeconds())+"."+
leadZero3(this.getMilliseconds());}
Object.defineProperty(Array.prototype,"remove",{value:function(value,firstOnly)
{var index=this.indexOf(value);if(index===-1)
return false;if(firstOnly){this.splice(index,1);return true;}
for(var i=index+1,n=this.length;i<n;++i){if(this[i]!==value)
this[index++]=this[i];}
this.length=index;return true;}});Object.defineProperty(Array.prototype,"keySet",{value:function()
{var keys={};for(var i=0;i<this.length;++i)
keys[this[i]]=true;return keys;}});Object.defineProperty(Array.prototype,"pushAll",{value:function(array)
{Array.prototype.push.apply(this,array);}});Object.defineProperty(Array.prototype,"rotate",{value:function(index)
{var result=[];for(var i=index;i<index+this.length;++i)
result.push(this[i%this.length]);return result;}});Object.defineProperty(Array.prototype,"sortNumbers",{value:function()
{function numericComparator(a,b)
{return a-b;}
this.sort(numericComparator);}});Object.defineProperty(Uint32Array.prototype,"sort",{value:Array.prototype.sort});(function(){var partition={value:function(comparator,left,right,pivotIndex)
{function swap(array,i1,i2)
{var temp=array[i1];array[i1]=array[i2];array[i2]=temp;}
var pivotValue=this[pivotIndex];swap(this,right,pivotIndex);var storeIndex=left;for(var i=left;i<right;++i){if(comparator(this[i],pivotValue)<0){swap(this,storeIndex,i);++storeIndex;}}
swap(this,right,storeIndex);return storeIndex;}};Object.defineProperty(Array.prototype,"partition",partition);Object.defineProperty(Uint32Array.prototype,"partition",partition);var sortRange={value:function(comparator,leftBound,rightBound,sortWindowLeft,sortWindowRight)
{function quickSortRange(array,comparator,left,right,sortWindowLeft,sortWindowRight)
{if(right<=left)
return;var pivotIndex=Math.floor(Math.random()*(right-left))+left;var pivotNewIndex=array.partition(comparator,left,right,pivotIndex);if(sortWindowLeft<pivotNewIndex)
quickSortRange(array,comparator,left,pivotNewIndex-1,sortWindowLeft,sortWindowRight);if(pivotNewIndex<sortWindowRight)
quickSortRange(array,comparator,pivotNewIndex+1,right,sortWindowLeft,sortWindowRight);}
if(leftBound===0&&rightBound===(this.length-1)&&sortWindowLeft===0&&sortWindowRight>=rightBound)
this.sort(comparator);else
quickSortRange(this,comparator,leftBound,rightBound,sortWindowLeft,sortWindowRight);return this;}}
Object.defineProperty(Array.prototype,"sortRange",sortRange);Object.defineProperty(Uint32Array.prototype,"sortRange",sortRange);})();Object.defineProperty(Array.prototype,"stableSort",{value:function(comparator)
{function defaultComparator(a,b)
{return a<b?-1:(a>b?1:0);}
comparator=comparator||defaultComparator;var indices=new Array(this.length);for(var i=0;i<this.length;++i)
indices[i]=i;var self=this;function indexComparator(a,b)
{var result=comparator(self[a],self[b]);return result?result:a-b;}
indices.sort(indexComparator);for(var i=0;i<this.length;++i){if(indices[i]<0||i===indices[i])
continue;var cyclical=i;var saved=this[i];while(true){var next=indices[cyclical];indices[cyclical]=-1;if(next===i){this[cyclical]=saved;break;}else{this[cyclical]=this[next];cyclical=next;}}}
return this;}});Object.defineProperty(Array.prototype,"qselect",{value:function(k,comparator)
{if(k<0||k>=this.length)
return;if(!comparator)
comparator=function(a,b){return a-b;}
var low=0;var high=this.length-1;for(;;){var pivotPosition=this.partition(comparator,low,high,Math.floor((high+low)/2));if(pivotPosition===k)
return this[k];else if(pivotPosition>k)
high=pivotPosition-1;else
low=pivotPosition+1;}}});Object.defineProperty(Array.prototype,"lowerBound",{value:function(object,comparator,left,right)
{function defaultComparator(a,b)
{return a<b?-1:(a>b?1:0);}
comparator=comparator||defaultComparator;var l=left||0;var r=right!==undefined?right:this.length;while(l<r){var m=(l+r)>>1;if(comparator(object,this[m])>0)
l=m+1;else
r=m;}
return r;}});Object.defineProperty(Array.prototype,"upperBound",{value:function(object,comparator,left,right)
{function defaultComparator(a,b)
{return a<b?-1:(a>b?1:0);}
comparator=comparator||defaultComparator;var l=left||0;var r=right!==undefined?right:this.length;while(l<r){var m=(l+r)>>1;if(comparator(object,this[m])>=0)
l=m+1;else
r=m;}
return r;}});Object.defineProperty(Uint32Array.prototype,"lowerBound",{value:Array.prototype.lowerBound});Object.defineProperty(Uint32Array.prototype,"upperBound",{value:Array.prototype.upperBound});Object.defineProperty(Float64Array.prototype,"lowerBound",{value:Array.prototype.lowerBound});Object.defineProperty(Array.prototype,"binaryIndexOf",{value:function(value,comparator)
{var index=this.lowerBound(value,comparator);return index<this.length&&comparator(value,this[index])===0?index:-1;}});Object.defineProperty(Array.prototype,"select",{value:function(field)
{var result=new Array(this.length);for(var i=0;i<this.length;++i)
result[i]=this[i][field];return result;}});Object.defineProperty(Array.prototype,"peekLast",{value:function()
{return this[this.length-1];}});(function(){function mergeOrIntersect(array1,array2,comparator,mergeNotIntersect)
{var result=[];var i=0;var j=0;while(i<array1.length&&j<array2.length){var compareValue=comparator(array1[i],array2[j]);if(mergeNotIntersect||!compareValue)
result.push(compareValue<=0?array1[i]:array2[j]);if(compareValue<=0)
i++;if(compareValue>=0)
j++;}
if(mergeNotIntersect){while(i<array1.length)
result.push(array1[i++]);while(j<array2.length)
result.push(array2[j++]);}
return result;}
Object.defineProperty(Array.prototype,"intersectOrdered",{value:function(array,comparator)
{return mergeOrIntersect(this,array,comparator,false);}});Object.defineProperty(Array.prototype,"mergeOrdered",{value:function(array,comparator)
{return mergeOrIntersect(this,array,comparator,true);}});}());function insertionIndexForObjectInListSortedByFunction(object,list,comparator,insertionIndexAfter)
{if(insertionIndexAfter)
return list.upperBound(object,comparator);else
return list.lowerBound(object,comparator);}
String.sprintf=function(format,var_arg)
{return String.vsprintf(format,Array.prototype.slice.call(arguments,1));}
String.tokenizeFormatString=function(format,formatters)
{var tokens=[];var substitutionIndex=0;function addStringToken(str)
{if(tokens.length&&tokens[tokens.length-1].type==="string")
tokens[tokens.length-1].value+=str;else
tokens.push({type:"string",value:str});}
function addSpecifierToken(specifier,precision,substitutionIndex)
{tokens.push({type:"specifier",specifier:specifier,precision:precision,substitutionIndex:substitutionIndex});}
var index=0;for(var precentIndex=format.indexOf("%",index);precentIndex!==-1;precentIndex=format.indexOf("%",index)){if(format.length===index)
break;addStringToken(format.substring(index,precentIndex));index=precentIndex+1;if(format[index]==="%"){addStringToken("%");++index;continue;}
if(String.isDigitAt(format,index)){var number=parseInt(format.substring(index),10);while(String.isDigitAt(format,index))
++index;if(number>0&&format[index]==="$"){substitutionIndex=(number-1);++index;}}
var precision=-1;if(format[index]==="."){++index;precision=parseInt(format.substring(index),10);if(isNaN(precision))
precision=0;while(String.isDigitAt(format,index))
++index;}
if(!(format[index]in formatters)){addStringToken(format.substring(precentIndex,index+1));++index;continue;}
addSpecifierToken(format[index],precision,substitutionIndex);++substitutionIndex;++index;}
addStringToken(format.substring(index));return tokens;}
String.standardFormatters={d:function(substitution)
{return!isNaN(substitution)?substitution:0;},f:function(substitution,token)
{if(substitution&&token.precision>-1)
substitution=substitution.toFixed(token.precision);return!isNaN(substitution)?substitution:(token.precision>-1?Number(0).toFixed(token.precision):0);},s:function(substitution)
{return substitution;}}
String.vsprintf=function(format,substitutions)
{return String.format(format,substitutions,String.standardFormatters,"",function(a,b){return a+b;}).formattedResult;}
String.format=function(format,substitutions,formatters,initialValue,append,tokenizedFormat)
{if(!format||!substitutions||!substitutions.length)
return{formattedResult:append(initialValue,format),unusedSubstitutions:substitutions};function prettyFunctionName()
{return"String.format(\""+format+"\", \""+Array.prototype.join.call(substitutions,"\", \"")+"\")";}
function warn(msg)
{console.warn(prettyFunctionName()+": "+msg);}
function error(msg)
{console.error(prettyFunctionName()+": "+msg);}
var result=initialValue;var tokens=tokenizedFormat||String.tokenizeFormatString(format,formatters);var usedSubstitutionIndexes={};for(var i=0;i<tokens.length;++i){var token=tokens[i];if(token.type==="string"){result=append(result,token.value);continue;}
if(token.type!=="specifier"){error("Unknown token type \""+token.type+"\" found.");continue;}
if(token.substitutionIndex>=substitutions.length){error("not enough substitution arguments. Had "+substitutions.length+" but needed "+(token.substitutionIndex+1)+", so substitution was skipped.");result=append(result,"%"+(token.precision>-1?token.precision:"")+token.specifier);continue;}
usedSubstitutionIndexes[token.substitutionIndex]=true;if(!(token.specifier in formatters)){warn("unsupported format character \u201C"+token.specifier+"\u201D. Treating as a string.");result=append(result,substitutions[token.substitutionIndex]);continue;}
result=append(result,formatters[token.specifier](substitutions[token.substitutionIndex],token));}
var unusedSubstitutions=[];for(var i=0;i<substitutions.length;++i){if(i in usedSubstitutionIndexes)
continue;unusedSubstitutions.push(substitutions[i]);}
return{formattedResult:result,unusedSubstitutions:unusedSubstitutions};}
function createSearchRegex(query,caseSensitive,isRegex)
{var regexFlags=caseSensitive?"g":"gi";var regexObject;if(isRegex){try{regexObject=new RegExp(query,regexFlags);}catch(e){}}
if(!regexObject)
regexObject=createPlainTextSearchRegex(query,regexFlags);return regexObject;}
function createPlainTextSearchRegex(query,flags)
{var regexSpecialCharacters=String.regexSpecialCharacters();var regex="";for(var i=0;i<query.length;++i){var c=query.charAt(i);if(regexSpecialCharacters.indexOf(c)!=-1)
regex+="\\";regex+=c;}
return new RegExp(regex,flags||"");}
function countRegexMatches(regex,content)
{var text=content;var result=0;var match;while(text&&(match=regex.exec(text))){if(match[0].length>0)
++result;text=text.substring(match.index+1);}
return result;}
function spacesPadding(spacesCount)
{return"\u00a0".repeat(spacesCount);}
function numberToStringWithSpacesPadding(value,symbolsCount)
{var numberString=value.toString();var paddingLength=Math.max(0,symbolsCount-numberString.length);return spacesPadding(paddingLength)+numberString;}
Array.from=function(iterator)
{var values=[];for(var iteratorValue=iterator.next();!iteratorValue.done;iteratorValue=iterator.next())
values.push(iteratorValue.value);return values;}
Set.prototype.valuesArray=function()
{return Array.from(this.values());}
Map.prototype.remove=function(key)
{var value=this.get(key);this.delete(key);return value;}
Map.prototype.valuesArray=function()
{return Array.from(this.values());}
Map.prototype.keysArray=function()
{return Array.from(this.keys());}
var Multimap=function()
{this._map=new Map();}
Multimap.prototype={set:function(key,value)
{var set=this._map.get(key);if(!set){set=new Set();this._map.set(key,set);}
set.add(value);},get:function(key)
{var result=this._map.get(key);if(!result)
result=new Set();return result;},remove:function(key,value)
{var values=this.get(key);values.delete(value);if(!values.size)
this._map.delete(key);},removeAll:function(key)
{this._map.delete(key);},keysArray:function()
{return this._map.keysArray();},valuesArray:function()
{var result=[];var keys=this.keysArray();for(var i=0;i<keys.length;++i)
result.pushAll(this.get(keys[i]).valuesArray());return result;},clear:function()
{this._map.clear();}}
function loadXHR(url)
{return new Promise(load);function load(successCallback,failureCallback)
{function onReadyStateChanged()
{if(xhr.readyState!==XMLHttpRequest.DONE)
return;if(xhr.status!==200){xhr.onreadystatechange=null;failureCallback(new Error(xhr.status));return;}
xhr.onreadystatechange=null;successCallback(xhr.responseText);}
var xhr=new XMLHttpRequest();xhr.withCredentials=false;xhr.open("GET",url,true);xhr.onreadystatechange=onReadyStateChanged;xhr.send(null);}}
function CallbackBarrier()
{this._pendingIncomingCallbacksCount=0;}
CallbackBarrier.prototype={createCallback:function(userCallback)
{console.assert(!this._outgoingCallback,"CallbackBarrier.createCallback() is called after CallbackBarrier.callWhenDone()");++this._pendingIncomingCallbacksCount;return this._incomingCallback.bind(this,userCallback);},callWhenDone:function(callback)
{console.assert(!this._outgoingCallback,"CallbackBarrier.callWhenDone() is called multiple times");this._outgoingCallback=callback;if(!this._pendingIncomingCallbacksCount)
this._outgoingCallback();},donePromise:function()
{return new Promise(promiseConstructor.bind(this));function promiseConstructor(success)
{this.callWhenDone(success);}},_incomingCallback:function(userCallback)
{console.assert(this._pendingIncomingCallbacksCount>0);if(userCallback){var args=Array.prototype.slice.call(arguments,1);userCallback.apply(null,args);}
if(!--this._pendingIncomingCallbacksCount&&this._outgoingCallback)
this._outgoingCallback();}}
function suppressUnused(value)
{}
self.setImmediate=function(callback)
{Promise.resolve().then(callback);return 0;}
Promise.prototype.spread=function(callback)
{return this.then(spreadPromise);function spreadPromise(arg)
{return callback.apply(null,arg);}}
Promise.prototype.catchException=function(defaultValue){return this.catch(function(error){console.error(error);return defaultValue;});};self.WebInspector={};WebInspector.UIString=function(string,vararg)
{return String.vsprintf(WebInspector.localize(string),Array.prototype.slice.call(arguments,1));}
WebInspector.UIString.capitalize=function(string,vararg)
{if(WebInspector._useLowerCaseMenuTitles===undefined)
throw"WebInspector.setLocalizationPlatform() has not been called";var localized=WebInspector.localize(string);var capitalized;if(WebInspector._useLowerCaseMenuTitles)
capitalized=localized.replace(/\^(.)/g,"$1");else
capitalized=localized.replace(/\^(.)/g,function(str,char){return char.toUpperCase();});return String.vsprintf(capitalized,Array.prototype.slice.call(arguments,1));}
WebInspector.setLocalizationPlatform=function(platform)
{WebInspector._useLowerCaseMenuTitles=platform==="windows";}
WebInspector.localize=function(string)
{return string;}
WebInspector.UIStringFormat=function(format)
{this._localizedFormat=WebInspector.localize(format);this._tokenizedFormat=String.tokenizeFormatString(this._localizedFormat,String.standardFormatters);}
WebInspector.UIStringFormat._append=function(a,b)
{return a+b;}
WebInspector.UIStringFormat.prototype={format:function(vararg)
{return String.format(this._localizedFormat,arguments,String.standardFormatters,"",WebInspector.UIStringFormat._append,this._tokenizedFormat).formattedResult;}};WebInspector.TextUtils={isStopChar:function(char)
{return(char>" "&&char<"0")||(char>"9"&&char<"A")||(char>"Z"&&char<"_")||(char>"_"&&char<"a")||(char>"z"&&char<="~");},isWordChar:function(char)
{return!WebInspector.TextUtils.isStopChar(char)&&!WebInspector.TextUtils.isSpaceChar(char);},isSpaceChar:function(char)
{return WebInspector.TextUtils._SpaceCharRegex.test(char);},isWord:function(word)
{for(var i=0;i<word.length;++i){if(!WebInspector.TextUtils.isWordChar(word.charAt(i)))
return false;}
return true;},isOpeningBraceChar:function(char)
{return char==="("||char==="{";},isClosingBraceChar:function(char)
{return char===")"||char==="}";},isBraceChar:function(char)
{return WebInspector.TextUtils.isOpeningBraceChar(char)||WebInspector.TextUtils.isClosingBraceChar(char);},textToWords:function(text,isWordChar,wordCallback)
{var startWord=-1;for(var i=0;i<text.length;++i){if(!isWordChar(text.charAt(i))){if(startWord!==-1)
wordCallback(text.substring(startWord,i));startWord=-1;}else if(startWord===-1)
startWord=i;}
if(startWord!==-1)
wordCallback(text.substring(startWord));},lineIndent:function(line)
{var indentation=0;while(indentation<line.length&&WebInspector.TextUtils.isSpaceChar(line.charAt(indentation)))
++indentation;return line.substr(0,indentation);},isUpperCase:function(text)
{return text===text.toUpperCase();},isLowerCase:function(text)
{return text===text.toLowerCase();}}
WebInspector.TextUtils._SpaceCharRegex=/\s/;WebInspector.TextUtils.Indent={TwoSpaces:"  ",FourSpaces:"    ",EightSpaces:"        ",TabCharacter:"\t"}
WebInspector.TextUtils.BalancedJSONTokenizer=function(callback,findMultiple)
{this._callback=callback;this._index=0;this._balance=0;this._buffer="";this._findMultiple=findMultiple||false;this._closingDoubleQuoteRegex=/[^\\](?:\\\\)*"/g;}
WebInspector.TextUtils.BalancedJSONTokenizer.prototype={write:function(chunk)
{this._buffer+=chunk;var lastIndex=this._buffer.length;var buffer=this._buffer;for(var index=this._index;index<lastIndex;++index){var character=buffer[index];if(character==="\""){this._closingDoubleQuoteRegex.lastIndex=index;if(!this._closingDoubleQuoteRegex.test(buffer))
break;index=this._closingDoubleQuoteRegex.lastIndex-1;}else if(character==="{"){++this._balance;}else if(character==="}"){if(--this._balance===0){this._lastBalancedIndex=index+1;if(!this._findMultiple)
break;}}}
this._index=index;this._reportBalanced();},_reportBalanced:function()
{if(!this._lastBalancedIndex)
return;this._callback(this._buffer.slice(0,this._lastBalancedIndex));this._buffer=this._buffer.slice(this._lastBalancedIndex);this._index-=this._lastBalancedIndex;this._lastBalancedIndex=0;},remainder:function()
{return this._buffer;}}
WebInspector.TokenizerFactory=function(){}
WebInspector.TokenizerFactory.prototype={createTokenizer:function(mimeType){}};WebInspector.HeapSnapshotProgressEvent={Update:"ProgressUpdate",BrokenSnapshot:"BrokenSnapshot"};WebInspector.HeapSnapshotCommon={}
WebInspector.HeapSnapshotCommon.baseSystemDistance=100000000;WebInspector.HeapSnapshotCommon.AllocationNodeCallers=function(nodesWithSingleCaller,branchingCallers)
{this.nodesWithSingleCaller=nodesWithSingleCaller;this.branchingCallers=branchingCallers;}
WebInspector.HeapSnapshotCommon.SerializedAllocationNode=function(nodeId,functionName,scriptName,scriptId,line,column,count,size,liveCount,liveSize,hasChildren)
{this.id=nodeId;this.name=functionName;this.scriptName=scriptName;this.scriptId=scriptId;this.line=line;this.column=column;this.count=count;this.size=size;this.liveCount=liveCount;this.liveSize=liveSize;this.hasChildren=hasChildren;}
WebInspector.HeapSnapshotCommon.AllocationStackFrame=function(functionName,scriptName,scriptId,line,column)
{this.functionName=functionName;this.scriptName=scriptName;this.scriptId=scriptId;this.line=line;this.column=column;}
WebInspector.HeapSnapshotCommon.Node=function(id,name,distance,nodeIndex,retainedSize,selfSize,type)
{this.id=id;this.name=name;this.distance=distance;this.nodeIndex=nodeIndex;this.retainedSize=retainedSize;this.selfSize=selfSize;this.type=type;this.canBeQueried=false;this.detachedDOMTreeNode=false;}
WebInspector.HeapSnapshotCommon.Edge=function(name,node,type,edgeIndex)
{this.name=name;this.node=node;this.type=type;this.edgeIndex=edgeIndex;};WebInspector.HeapSnapshotCommon.Aggregate=function()
{this.count;this.distance;this.self;this.maxRet;this.type;this.name;this.idxs;}
WebInspector.HeapSnapshotCommon.AggregateForDiff=function(){this.indexes=[];this.ids=[];this.selfSizes=[];}
WebInspector.HeapSnapshotCommon.Diff=function()
{this.addedCount=0;this.removedCount=0;this.addedSize=0;this.removedSize=0;this.deletedIndexes=[];this.addedIndexes=[];}
WebInspector.HeapSnapshotCommon.DiffForClass=function()
{this.addedCount;this.removedCount;this.addedSize;this.removedSize;this.deletedIndexes;this.addedIndexes;this.countDelta;this.sizeDelta;}
WebInspector.HeapSnapshotCommon.ComparatorConfig=function()
{this.fieldName1;this.ascending1;this.fieldName2;this.ascending2;}
WebInspector.HeapSnapshotCommon.WorkerCommand=function()
{this.callId;this.disposition;this.objectId;this.newObjectId;this.methodName;this.methodArguments;this.source;}
WebInspector.HeapSnapshotCommon.ItemsRange=function(startPosition,endPosition,totalLength,items)
{this.startPosition=startPosition;this.endPosition=endPosition;this.totalLength=totalLength;this.items=items;}
WebInspector.HeapSnapshotCommon.StaticData=function(nodeCount,rootNodeIndex,totalSize,maxJSObjectId)
{this.nodeCount=nodeCount;this.rootNodeIndex=rootNodeIndex;this.totalSize=totalSize;this.maxJSObjectId=maxJSObjectId;}
WebInspector.HeapSnapshotCommon.Statistics=function()
{this.total;this.v8heap;this.native;this.code;this.jsArrays;this.strings;this.system;}
WebInspector.HeapSnapshotCommon.NodeFilter=function(minNodeId,maxNodeId)
{this.minNodeId=minNodeId;this.maxNodeId=maxNodeId;this.allocationNodeId;}
WebInspector.HeapSnapshotCommon.NodeFilter.prototype={equals:function(o)
{return this.minNodeId===o.minNodeId&&this.maxNodeId===o.maxNodeId&&this.allocationNodeId===o.allocationNodeId;}}
WebInspector.HeapSnapshotCommon.SearchConfig=function(query,caseSensitive,isRegex,shouldJump,jumpBackward)
{this.query=query;this.caseSensitive=caseSensitive;this.isRegex=isRegex;this.shouldJump=shouldJump;this.jumpBackward=jumpBackward;}
WebInspector.HeapSnapshotCommon.Samples=function(timestamps,lastAssignedIds,sizes)
{this.timestamps=timestamps;this.lastAssignedIds=lastAssignedIds;this.sizes=sizes;};WebInspector.AllocationProfile=function(profile,liveObjectStats)
{this._strings=profile.strings;this._liveObjectStats=liveObjectStats;this._nextNodeId=1;this._functionInfos=[];this._idToNode={};this._idToTopDownNode={};this._collapsedTopNodeIdToFunctionInfo={};this._traceTops=null;this._buildFunctionAllocationInfos(profile);this._traceTree=this._buildAllocationTree(profile,liveObjectStats);}
WebInspector.AllocationProfile.prototype={_buildFunctionAllocationInfos:function(profile)
{var strings=this._strings;var functionInfoFields=profile.snapshot.meta.trace_function_info_fields;var functionNameOffset=functionInfoFields.indexOf("name");var scriptNameOffset=functionInfoFields.indexOf("script_name");var scriptIdOffset=functionInfoFields.indexOf("script_id");var lineOffset=functionInfoFields.indexOf("line");var columnOffset=functionInfoFields.indexOf("column");var functionInfoFieldCount=functionInfoFields.length;var rawInfos=profile.trace_function_infos;var infoLength=rawInfos.length;var functionInfos=this._functionInfos=new Array(infoLength/functionInfoFieldCount);var index=0;for(var i=0;i<infoLength;i+=functionInfoFieldCount){functionInfos[index++]=new WebInspector.FunctionAllocationInfo(strings[rawInfos[i+functionNameOffset]],strings[rawInfos[i+scriptNameOffset]],rawInfos[i+scriptIdOffset],rawInfos[i+lineOffset],rawInfos[i+columnOffset]);}},_buildAllocationTree:function(profile,liveObjectStats)
{var traceTreeRaw=profile.trace_tree;var functionInfos=this._functionInfos;var idToTopDownNode=this._idToTopDownNode;var traceNodeFields=profile.snapshot.meta.trace_node_fields;var nodeIdOffset=traceNodeFields.indexOf("id");var functionInfoIndexOffset=traceNodeFields.indexOf("function_info_index");var allocationCountOffset=traceNodeFields.indexOf("count");var allocationSizeOffset=traceNodeFields.indexOf("size");var childrenOffset=traceNodeFields.indexOf("children");var nodeFieldCount=traceNodeFields.length;function traverseNode(rawNodeArray,nodeOffset,parent)
{var functionInfo=functionInfos[rawNodeArray[nodeOffset+functionInfoIndexOffset]];var id=rawNodeArray[nodeOffset+nodeIdOffset];var stats=liveObjectStats[id];var liveCount=stats?stats.count:0;var liveSize=stats?stats.size:0;var result=new WebInspector.TopDownAllocationNode(id,functionInfo,rawNodeArray[nodeOffset+allocationCountOffset],rawNodeArray[nodeOffset+allocationSizeOffset],liveCount,liveSize,parent);idToTopDownNode[id]=result;functionInfo.addTraceTopNode(result);var rawChildren=rawNodeArray[nodeOffset+childrenOffset];for(var i=0;i<rawChildren.length;i+=nodeFieldCount){result.children.push(traverseNode(rawChildren,i,result));}
return result;}
return traverseNode(traceTreeRaw,0,null);},serializeTraceTops:function()
{if(this._traceTops)
return this._traceTops;var result=this._traceTops=[];var functionInfos=this._functionInfos;for(var i=0;i<functionInfos.length;i++){var info=functionInfos[i];if(info.totalCount===0)
continue;var nodeId=this._nextNodeId++;var isRoot=i==0;result.push(this._serializeNode(nodeId,info,info.totalCount,info.totalSize,info.totalLiveCount,info.totalLiveSize,!isRoot));this._collapsedTopNodeIdToFunctionInfo[nodeId]=info;}
result.sort(function(a,b){return b.size-a.size;});return result;},serializeCallers:function(nodeId)
{var node=this._ensureBottomUpNode(nodeId);var nodesWithSingleCaller=[];while(node.callers().length===1){node=node.callers()[0];nodesWithSingleCaller.push(this._serializeCaller(node));}
var branchingCallers=[];var callers=node.callers();for(var i=0;i<callers.length;i++){branchingCallers.push(this._serializeCaller(callers[i]));}
return new WebInspector.HeapSnapshotCommon.AllocationNodeCallers(nodesWithSingleCaller,branchingCallers);},serializeAllocationStack:function(traceNodeId)
{var node=this._idToTopDownNode[traceNodeId];var result=[];while(node){var functionInfo=node.functionInfo;result.push(new WebInspector.HeapSnapshotCommon.AllocationStackFrame(functionInfo.functionName,functionInfo.scriptName,functionInfo.scriptId,functionInfo.line,functionInfo.column));node=node.parent;}
return result;},traceIds:function(allocationNodeId)
{return this._ensureBottomUpNode(allocationNodeId).traceTopIds;},_ensureBottomUpNode:function(nodeId)
{var node=this._idToNode[nodeId];if(!node){var functionInfo=this._collapsedTopNodeIdToFunctionInfo[nodeId];node=functionInfo.bottomUpRoot();delete this._collapsedTopNodeIdToFunctionInfo[nodeId];this._idToNode[nodeId]=node;}
return node;},_serializeCaller:function(node)
{var callerId=this._nextNodeId++;this._idToNode[callerId]=node;return this._serializeNode(callerId,node.functionInfo,node.allocationCount,node.allocationSize,node.liveCount,node.liveSize,node.hasCallers());},_serializeNode:function(nodeId,functionInfo,count,size,liveCount,liveSize,hasChildren)
{return new WebInspector.HeapSnapshotCommon.SerializedAllocationNode(nodeId,functionInfo.functionName,functionInfo.scriptName,functionInfo.scriptId,functionInfo.line,functionInfo.column,count,size,liveCount,liveSize,hasChildren);}}
WebInspector.TopDownAllocationNode=function(id,functionInfo,count,size,liveCount,liveSize,parent)
{this.id=id;this.functionInfo=functionInfo;this.allocationCount=count;this.allocationSize=size;this.liveCount=liveCount;this.liveSize=liveSize;this.parent=parent;this.children=[];}
WebInspector.BottomUpAllocationNode=function(functionInfo)
{this.functionInfo=functionInfo;this.allocationCount=0;this.allocationSize=0;this.liveCount=0;this.liveSize=0;this.traceTopIds=[];this._callers=[];}
WebInspector.BottomUpAllocationNode.prototype={addCaller:function(traceNode)
{var functionInfo=traceNode.functionInfo;var result;for(var i=0;i<this._callers.length;i++){var caller=this._callers[i];if(caller.functionInfo===functionInfo){result=caller;break;}}
if(!result){result=new WebInspector.BottomUpAllocationNode(functionInfo);this._callers.push(result);}
return result;},callers:function()
{return this._callers;},hasCallers:function()
{return this._callers.length>0;}}
WebInspector.FunctionAllocationInfo=function(functionName,scriptName,scriptId,line,column)
{this.functionName=functionName;this.scriptName=scriptName;this.scriptId=scriptId;this.line=line;this.column=column;this.totalCount=0;this.totalSize=0;this.totalLiveCount=0;this.totalLiveSize=0;this._traceTops=[];}
WebInspector.FunctionAllocationInfo.prototype={addTraceTopNode:function(node)
{if(node.allocationCount===0)
return;this._traceTops.push(node);this.totalCount+=node.allocationCount;this.totalSize+=node.allocationSize;this.totalLiveCount+=node.liveCount;this.totalLiveSize+=node.liveSize;},bottomUpRoot:function()
{if(!this._traceTops.length)
return null;if(!this._bottomUpTree)
this._buildAllocationTraceTree();return this._bottomUpTree;},_buildAllocationTraceTree:function()
{this._bottomUpTree=new WebInspector.BottomUpAllocationNode(this);for(var i=0;i<this._traceTops.length;i++){var node=this._traceTops[i];var bottomUpNode=this._bottomUpTree;var count=node.allocationCount;var size=node.allocationSize;var liveCount=node.liveCount;var liveSize=node.liveSize;var traceId=node.id;while(true){bottomUpNode.allocationCount+=count;bottomUpNode.allocationSize+=size;bottomUpNode.liveCount+=liveCount;bottomUpNode.liveSize+=liveSize;bottomUpNode.traceTopIds.push(traceId);node=node.parent;if(node===null){break;}
bottomUpNode=bottomUpNode.addCaller(node);}}}};WebInspector.HeapSnapshotItem=function(){}
WebInspector.HeapSnapshotItem.prototype={itemIndex:function(){},serialize:function(){}};WebInspector.HeapSnapshotEdge=function(snapshot,edgeIndex)
{this._snapshot=snapshot;this._edges=snapshot.containmentEdges;this.edgeIndex=edgeIndex||0;}
WebInspector.HeapSnapshotEdge.prototype={clone:function()
{return new WebInspector.HeapSnapshotEdge(this._snapshot,this.edgeIndex);},hasStringName:function()
{throw new Error("Not implemented");},name:function()
{throw new Error("Not implemented");},node:function()
{return this._snapshot.createNode(this.nodeIndex());},nodeIndex:function()
{return this._edges[this.edgeIndex+this._snapshot._edgeToNodeOffset];},toString:function()
{return"HeapSnapshotEdge: "+this.name();},type:function()
{return this._snapshot._edgeTypes[this._type()];},itemIndex:function()
{return this.edgeIndex;},serialize:function()
{return new WebInspector.HeapSnapshotCommon.Edge(this.name(),this.node().serialize(),this.type(),this.edgeIndex);},_type:function()
{return this._edges[this.edgeIndex+this._snapshot._edgeTypeOffset];}};WebInspector.HeapSnapshotItemIterator=function(){}
WebInspector.HeapSnapshotItemIterator.prototype={hasNext:function(){},item:function(){},next:function(){}};WebInspector.HeapSnapshotItemIndexProvider=function(){}
WebInspector.HeapSnapshotItemIndexProvider.prototype={itemForIndex:function(newIndex){},};WebInspector.HeapSnapshotNodeIndexProvider=function(snapshot)
{this._node=snapshot.createNode();}
WebInspector.HeapSnapshotNodeIndexProvider.prototype={itemForIndex:function(index)
{this._node.nodeIndex=index;return this._node;}};WebInspector.HeapSnapshotEdgeIndexProvider=function(snapshot)
{this._edge=snapshot.createEdge(0);}
WebInspector.HeapSnapshotEdgeIndexProvider.prototype={itemForIndex:function(index)
{this._edge.edgeIndex=index;return this._edge;}};WebInspector.HeapSnapshotRetainerEdgeIndexProvider=function(snapshot)
{this._retainerEdge=snapshot.createRetainingEdge(0);}
WebInspector.HeapSnapshotRetainerEdgeIndexProvider.prototype={itemForIndex:function(index)
{this._retainerEdge.setRetainerIndex(index);return this._retainerEdge;}};WebInspector.HeapSnapshotEdgeIterator=function(node)
{this._sourceNode=node;this.edge=node._snapshot.createEdge(node.edgeIndexesStart());}
WebInspector.HeapSnapshotEdgeIterator.prototype={hasNext:function()
{return this.edge.edgeIndex<this._sourceNode.edgeIndexesEnd();},item:function()
{return this.edge;},next:function()
{this.edge.edgeIndex+=this.edge._snapshot._edgeFieldsCount;}};WebInspector.HeapSnapshotRetainerEdge=function(snapshot,retainerIndex)
{this._snapshot=snapshot;this.setRetainerIndex(retainerIndex);}
WebInspector.HeapSnapshotRetainerEdge.prototype={clone:function()
{return new WebInspector.HeapSnapshotRetainerEdge(this._snapshot,this.retainerIndex());},hasStringName:function()
{return this._edge().hasStringName();},name:function()
{return this._edge().name();},node:function()
{return this._node();},nodeIndex:function()
{return this._retainingNodeIndex;},retainerIndex:function()
{return this._retainerIndex;},setRetainerIndex:function(retainerIndex)
{if(retainerIndex===this._retainerIndex)
return;this._retainerIndex=retainerIndex;this._globalEdgeIndex=this._snapshot._retainingEdges[retainerIndex];this._retainingNodeIndex=this._snapshot._retainingNodes[retainerIndex];this._edgeInstance=null;this._nodeInstance=null;},set edgeIndex(edgeIndex)
{this.setRetainerIndex(edgeIndex);},_node:function()
{if(!this._nodeInstance)
this._nodeInstance=this._snapshot.createNode(this._retainingNodeIndex);return this._nodeInstance;},_edge:function()
{if(!this._edgeInstance)
this._edgeInstance=this._snapshot.createEdge(this._globalEdgeIndex);return this._edgeInstance;},toString:function()
{return this._edge().toString();},itemIndex:function()
{return this._retainerIndex;},serialize:function()
{return new WebInspector.HeapSnapshotCommon.Edge(this.name(),this.node().serialize(),this.type(),this._globalEdgeIndex);},type:function()
{return this._edge().type();}}
WebInspector.HeapSnapshotRetainerEdgeIterator=function(retainedNode)
{var snapshot=retainedNode._snapshot;var retainedNodeOrdinal=retainedNode.ordinal();var retainerIndex=snapshot._firstRetainerIndex[retainedNodeOrdinal];this._retainersEnd=snapshot._firstRetainerIndex[retainedNodeOrdinal+1];this.retainer=snapshot.createRetainingEdge(retainerIndex);}
WebInspector.HeapSnapshotRetainerEdgeIterator.prototype={hasNext:function()
{return this.retainer.retainerIndex()<this._retainersEnd;},item:function()
{return this.retainer;},next:function()
{this.retainer.setRetainerIndex(this.retainer.retainerIndex()+1);}};WebInspector.HeapSnapshotNode=function(snapshot,nodeIndex)
{this._snapshot=snapshot;this.nodeIndex=nodeIndex||0;}
WebInspector.HeapSnapshotNode.prototype={distance:function()
{return this._snapshot._nodeDistances[this.nodeIndex/this._snapshot._nodeFieldCount];},className:function()
{throw new Error("Not implemented");},classIndex:function()
{throw new Error("Not implemented");},dominatorIndex:function()
{var nodeFieldCount=this._snapshot._nodeFieldCount;return this._snapshot._dominatorsTree[this.nodeIndex/this._snapshot._nodeFieldCount]*nodeFieldCount;},edges:function()
{return new WebInspector.HeapSnapshotEdgeIterator(this);},edgesCount:function()
{return(this.edgeIndexesEnd()-this.edgeIndexesStart())/this._snapshot._edgeFieldsCount;},id:function()
{throw new Error("Not implemented");},isRoot:function()
{return this.nodeIndex===this._snapshot._rootNodeIndex;},name:function()
{return this._snapshot.strings[this._name()];},retainedSize:function()
{return this._snapshot._retainedSizes[this.ordinal()];},retainers:function()
{return new WebInspector.HeapSnapshotRetainerEdgeIterator(this);},retainersCount:function()
{var snapshot=this._snapshot;var ordinal=this.ordinal();return snapshot._firstRetainerIndex[ordinal+1]-snapshot._firstRetainerIndex[ordinal];},selfSize:function()
{var snapshot=this._snapshot;return snapshot.nodes[this.nodeIndex+snapshot._nodeSelfSizeOffset];},type:function()
{return this._snapshot._nodeTypes[this._type()];},traceNodeId:function()
{var snapshot=this._snapshot;return snapshot.nodes[this.nodeIndex+snapshot._nodeTraceNodeIdOffset];},itemIndex:function()
{return this.nodeIndex;},serialize:function()
{return new WebInspector.HeapSnapshotCommon.Node(this.id(),this.name(),this.distance(),this.nodeIndex,this.retainedSize(),this.selfSize(),this.type());},_name:function()
{var snapshot=this._snapshot;return snapshot.nodes[this.nodeIndex+snapshot._nodeNameOffset];},edgeIndexesStart:function()
{return this._snapshot._firstEdgeIndexes[this.ordinal()];},edgeIndexesEnd:function()
{return this._snapshot._firstEdgeIndexes[this.ordinal()+1];},ordinal:function()
{return this.nodeIndex/this._snapshot._nodeFieldCount;},_nextNodeIndex:function()
{return this.nodeIndex+this._snapshot._nodeFieldCount;},_type:function()
{var snapshot=this._snapshot;return snapshot.nodes[this.nodeIndex+snapshot._nodeTypeOffset];}};WebInspector.HeapSnapshotNodeIterator=function(node)
{this.node=node;this._nodesLength=node._snapshot.nodes.length;}
WebInspector.HeapSnapshotNodeIterator.prototype={hasNext:function()
{return this.node.nodeIndex<this._nodesLength;},item:function()
{return this.node;},next:function()
{this.node.nodeIndex=this.node._nextNodeIndex();}}
WebInspector.HeapSnapshotIndexRangeIterator=function(itemProvider,indexes)
{this._itemProvider=itemProvider;this._indexes=indexes;this._position=0;}
WebInspector.HeapSnapshotIndexRangeIterator.prototype={hasNext:function()
{return this._position<this._indexes.length;},item:function()
{var index=this._indexes[this._position];return this._itemProvider.itemForIndex(index);},next:function()
{++this._position;}}
WebInspector.HeapSnapshotFilteredIterator=function(iterator,filter)
{this._iterator=iterator;this._filter=filter;this._skipFilteredItems();}
WebInspector.HeapSnapshotFilteredIterator.prototype={hasNext:function()
{return this._iterator.hasNext();},item:function()
{return this._iterator.item();},next:function()
{this._iterator.next();this._skipFilteredItems();},_skipFilteredItems:function()
{while(this._iterator.hasNext()&&!this._filter(this._iterator.item())){this._iterator.next();}}}
WebInspector.HeapSnapshotProgress=function(dispatcher)
{this._dispatcher=dispatcher;}
WebInspector.HeapSnapshotProgress.prototype={updateStatus:function(status)
{this._sendUpdateEvent(WebInspector.UIString(status));},updateProgress:function(title,value,total)
{var percentValue=((total?(value/total):0)*100).toFixed(0);this._sendUpdateEvent(WebInspector.UIString(title,percentValue));},reportProblem:function(error)
{if(this._dispatcher)
this._dispatcher.sendEvent(WebInspector.HeapSnapshotProgressEvent.BrokenSnapshot,error);},_sendUpdateEvent:function(text)
{if(this._dispatcher)
this._dispatcher.sendEvent(WebInspector.HeapSnapshotProgressEvent.Update,text);}}
WebInspector.HeapSnapshotProblemReport=function(title)
{this._errors=[title];}
WebInspector.HeapSnapshotProblemReport.prototype={addError:function(error)
{if(this._errors.length>100)
return;this._errors.push(error);},toString:function()
{return this._errors.join("\n  ");}}
WebInspector.HeapSnapshot=function(profile,progress)
{this.nodes=profile.nodes;this.containmentEdges=profile.edges;this._metaNode=profile.snapshot.meta;this._rawSamples=profile.samples;this._samples=null;this.strings=profile.strings;this._progress=progress;this._noDistance=-5;this._rootNodeIndex=0;if(profile.snapshot.root_index)
this._rootNodeIndex=profile.snapshot.root_index;this._snapshotDiffs={};this._aggregatesForDiff=null;this._aggregates={};this._aggregatesSortedFlags={};this._init();if(profile.snapshot.trace_function_count){this._progress.updateStatus("Building allocation statistics\u2026");var nodes=this.nodes;var nodesLength=nodes.length;var nodeFieldCount=this._nodeFieldCount;var node=this.rootNode();var liveObjects={};for(var nodeIndex=0;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){node.nodeIndex=nodeIndex;var traceNodeId=node.traceNodeId();var stats=liveObjects[traceNodeId];if(!stats)
liveObjects[traceNodeId]=stats={count:0,size:0,ids:[]};stats.count++;stats.size+=node.selfSize();stats.ids.push(node.id());}
this._allocationProfile=new WebInspector.AllocationProfile(profile,liveObjects);this._progress.updateStatus("Done");}}
function HeapSnapshotMetainfo()
{this.node_fields=[];this.node_types=[];this.edge_fields=[];this.edge_types=[];this.trace_function_info_fields=[];this.trace_node_fields=[];this.sample_fields=[];this.type_strings={};}
function HeapSnapshotHeader()
{this.title="";this.meta=new HeapSnapshotMetainfo();this.node_count=0;this.edge_count=0;this.trace_function_count=0;}
WebInspector.HeapSnapshot.prototype={_init:function()
{var meta=this._metaNode;this._nodeTypeOffset=meta.node_fields.indexOf("type");this._nodeNameOffset=meta.node_fields.indexOf("name");this._nodeIdOffset=meta.node_fields.indexOf("id");this._nodeSelfSizeOffset=meta.node_fields.indexOf("self_size");this._nodeEdgeCountOffset=meta.node_fields.indexOf("edge_count");this._nodeTraceNodeIdOffset=meta.node_fields.indexOf("trace_node_id");this._nodeFieldCount=meta.node_fields.length;this._nodeTypes=meta.node_types[this._nodeTypeOffset];this._nodeArrayType=this._nodeTypes.indexOf("array");this._nodeHiddenType=this._nodeTypes.indexOf("hidden");this._nodeObjectType=this._nodeTypes.indexOf("object");this._nodeNativeType=this._nodeTypes.indexOf("native");this._nodeConsStringType=this._nodeTypes.indexOf("concatenated string");this._nodeSlicedStringType=this._nodeTypes.indexOf("sliced string");this._nodeCodeType=this._nodeTypes.indexOf("code");this._nodeSyntheticType=this._nodeTypes.indexOf("synthetic");this._edgeFieldsCount=meta.edge_fields.length;this._edgeTypeOffset=meta.edge_fields.indexOf("type");this._edgeNameOffset=meta.edge_fields.indexOf("name_or_index");this._edgeToNodeOffset=meta.edge_fields.indexOf("to_node");this._edgeTypes=meta.edge_types[this._edgeTypeOffset];this._edgeTypes.push("invisible");this._edgeElementType=this._edgeTypes.indexOf("element");this._edgeHiddenType=this._edgeTypes.indexOf("hidden");this._edgeInternalType=this._edgeTypes.indexOf("internal");this._edgeShortcutType=this._edgeTypes.indexOf("shortcut");this._edgeWeakType=this._edgeTypes.indexOf("weak");this._edgeInvisibleType=this._edgeTypes.indexOf("invisible");this.nodeCount=this.nodes.length/this._nodeFieldCount;this._edgeCount=this.containmentEdges.length/this._edgeFieldsCount;this._retainedSizes=new Float64Array(this.nodeCount);this._firstEdgeIndexes=new Uint32Array(this.nodeCount+1);this._retainingNodes=new Uint32Array(this._edgeCount);this._retainingEdges=new Uint32Array(this._edgeCount);this._firstRetainerIndex=new Uint32Array(this.nodeCount+1);this._nodeDistances=new Int32Array(this.nodeCount);this._firstDominatedNodeIndex=new Uint32Array(this.nodeCount+1);this._dominatedNodes=new Uint32Array(this.nodeCount-1);this._progress.updateStatus("Building edge indexes\u2026");this._buildEdgeIndexes();this._progress.updateStatus("Building retainers\u2026");this._buildRetainers();this._progress.updateStatus("Calculating node flags\u2026");this._calculateFlags();this._progress.updateStatus("Calculating distances\u2026");this.calculateDistances();this._progress.updateStatus("Building postorder index\u2026");var result=this._buildPostOrderIndex();this._progress.updateStatus("Building dominator tree\u2026");this._dominatorsTree=this._buildDominatorTree(result.postOrderIndex2NodeOrdinal,result.nodeOrdinal2PostOrderIndex);this._progress.updateStatus("Calculating retained sizes\u2026");this._calculateRetainedSizes(result.postOrderIndex2NodeOrdinal);this._progress.updateStatus("Buiding dominated nodes\u2026");this._buildDominatedNodes();this._progress.updateStatus("Calculating statistics\u2026");this._calculateStatistics();this._progress.updateStatus("Calculating samples\u2026");this._buildSamples();this._progress.updateStatus("Finished processing.");},_buildEdgeIndexes:function()
{var nodes=this.nodes;var nodeCount=this.nodeCount;var firstEdgeIndexes=this._firstEdgeIndexes;var nodeFieldCount=this._nodeFieldCount;var edgeFieldsCount=this._edgeFieldsCount;var nodeEdgeCountOffset=this._nodeEdgeCountOffset;firstEdgeIndexes[nodeCount]=this.containmentEdges.length;for(var nodeOrdinal=0,edgeIndex=0;nodeOrdinal<nodeCount;++nodeOrdinal){firstEdgeIndexes[nodeOrdinal]=edgeIndex;edgeIndex+=nodes[nodeOrdinal*nodeFieldCount+nodeEdgeCountOffset]*edgeFieldsCount;}},_buildRetainers:function()
{var retainingNodes=this._retainingNodes;var retainingEdges=this._retainingEdges;var firstRetainerIndex=this._firstRetainerIndex;var containmentEdges=this.containmentEdges;var edgeFieldsCount=this._edgeFieldsCount;var nodeFieldCount=this._nodeFieldCount;var edgeToNodeOffset=this._edgeToNodeOffset;var firstEdgeIndexes=this._firstEdgeIndexes;var nodeCount=this.nodeCount;for(var toNodeFieldIndex=edgeToNodeOffset,l=containmentEdges.length;toNodeFieldIndex<l;toNodeFieldIndex+=edgeFieldsCount){var toNodeIndex=containmentEdges[toNodeFieldIndex];if(toNodeIndex%nodeFieldCount)
throw new Error("Invalid toNodeIndex "+toNodeIndex);++firstRetainerIndex[toNodeIndex/nodeFieldCount];}
for(var i=0,firstUnusedRetainerSlot=0;i<nodeCount;i++){var retainersCount=firstRetainerIndex[i];firstRetainerIndex[i]=firstUnusedRetainerSlot;retainingNodes[firstUnusedRetainerSlot]=retainersCount;firstUnusedRetainerSlot+=retainersCount;}
firstRetainerIndex[nodeCount]=retainingNodes.length;var nextNodeFirstEdgeIndex=firstEdgeIndexes[0];for(var srcNodeOrdinal=0;srcNodeOrdinal<nodeCount;++srcNodeOrdinal){var firstEdgeIndex=nextNodeFirstEdgeIndex;nextNodeFirstEdgeIndex=firstEdgeIndexes[srcNodeOrdinal+1];var srcNodeIndex=srcNodeOrdinal*nodeFieldCount;for(var edgeIndex=firstEdgeIndex;edgeIndex<nextNodeFirstEdgeIndex;edgeIndex+=edgeFieldsCount){var toNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];if(toNodeIndex%nodeFieldCount)
throw new Error("Invalid toNodeIndex "+toNodeIndex);var firstRetainerSlotIndex=firstRetainerIndex[toNodeIndex/nodeFieldCount];var nextUnusedRetainerSlotIndex=firstRetainerSlotIndex+(--retainingNodes[firstRetainerSlotIndex]);retainingNodes[nextUnusedRetainerSlotIndex]=srcNodeIndex;retainingEdges[nextUnusedRetainerSlotIndex]=edgeIndex;}}},createNode:function(nodeIndex)
{throw new Error("Not implemented");},createEdge:function(edgeIndex)
{throw new Error("Not implemented");},createRetainingEdge:function(retainerIndex)
{throw new Error("Not implemented");},_allNodes:function()
{return new WebInspector.HeapSnapshotNodeIterator(this.rootNode());},rootNode:function()
{return this.createNode(this._rootNodeIndex);},get rootNodeIndex()
{return this._rootNodeIndex;},get totalSize()
{return this.rootNode().retainedSize();},_getDominatedIndex:function(nodeIndex)
{if(nodeIndex%this._nodeFieldCount)
throw new Error("Invalid nodeIndex: "+nodeIndex);return this._firstDominatedNodeIndex[nodeIndex/this._nodeFieldCount];},_createFilter:function(nodeFilter)
{var minNodeId=nodeFilter.minNodeId;var maxNodeId=nodeFilter.maxNodeId;var allocationNodeId=nodeFilter.allocationNodeId;var filter;if(typeof allocationNodeId==="number"){filter=this._createAllocationStackFilter(allocationNodeId);filter.key="AllocationNodeId: "+allocationNodeId;}else if(typeof minNodeId==="number"&&typeof maxNodeId==="number"){filter=this._createNodeIdFilter(minNodeId,maxNodeId);filter.key="NodeIdRange: "+minNodeId+".."+maxNodeId;}
return filter;},search:function(searchConfig,nodeFilter)
{var query=searchConfig.query;function filterString(matchedStringIndexes,string,index)
{if(string.indexOf(query)!==-1)
matchedStringIndexes.add(index);return matchedStringIndexes;}
var regexp=searchConfig.isRegex?new RegExp(query):createPlainTextSearchRegex(query,"i");function filterRegexp(matchedStringIndexes,string,index)
{if(regexp.test(string))
matchedStringIndexes.add(index);return matchedStringIndexes;}
var stringFilter=(searchConfig.isRegex||!searchConfig.caseSensitive)?filterRegexp:filterString;var stringIndexes=this.strings.reduce(stringFilter,new Set());if(!stringIndexes.size)
return[];var filter=this._createFilter(nodeFilter);var nodeIds=[];var nodesLength=this.nodes.length;var nodes=this.nodes;var nodeNameOffset=this._nodeNameOffset;var nodeIdOffset=this._nodeIdOffset;var nodeFieldCount=this._nodeFieldCount;var node=this.rootNode();for(var nodeIndex=0;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){node.nodeIndex=nodeIndex;if(filter&&!filter(node))
continue;if(stringIndexes.has(nodes[nodeIndex+nodeNameOffset]))
nodeIds.push(nodes[nodeIndex+nodeIdOffset]);}
return nodeIds;},aggregatesWithFilter:function(nodeFilter)
{var filter=this._createFilter(nodeFilter);var key=filter?filter.key:"allObjects";return this.aggregates(false,key,filter);},_createNodeIdFilter:function(minNodeId,maxNodeId)
{function nodeIdFilter(node)
{var id=node.id();return id>minNodeId&&id<=maxNodeId;}
return nodeIdFilter;},_createAllocationStackFilter:function(bottomUpAllocationNodeId)
{var traceIds=this._allocationProfile.traceIds(bottomUpAllocationNodeId);if(!traceIds.length)
return undefined;var set={};for(var i=0;i<traceIds.length;i++)
set[traceIds[i]]=true;function traceIdFilter(node)
{return!!set[node.traceNodeId()];};return traceIdFilter;},aggregates:function(sortedIndexes,key,filter)
{var aggregatesByClassName=key&&this._aggregates[key];if(!aggregatesByClassName){var aggregates=this._buildAggregates(filter);this._calculateClassesRetainedSize(aggregates.aggregatesByClassIndex,filter);aggregatesByClassName=aggregates.aggregatesByClassName;if(key)
this._aggregates[key]=aggregatesByClassName;}
if(sortedIndexes&&(!key||!this._aggregatesSortedFlags[key])){this._sortAggregateIndexes(aggregatesByClassName);if(key)
this._aggregatesSortedFlags[key]=sortedIndexes;}
return aggregatesByClassName;},allocationTracesTops:function()
{return this._allocationProfile.serializeTraceTops();},allocationNodeCallers:function(nodeId)
{return this._allocationProfile.serializeCallers(nodeId);},allocationStack:function(nodeIndex)
{var node=this.createNode(nodeIndex);var allocationNodeId=node.traceNodeId();if(!allocationNodeId)
return null;return this._allocationProfile.serializeAllocationStack(allocationNodeId);},aggregatesForDiff:function()
{if(this._aggregatesForDiff)
return this._aggregatesForDiff;var aggregatesByClassName=this.aggregates(true,"allObjects");this._aggregatesForDiff={};var node=this.createNode();for(var className in aggregatesByClassName){var aggregate=aggregatesByClassName[className];var indexes=aggregate.idxs;var ids=new Array(indexes.length);var selfSizes=new Array(indexes.length);for(var i=0;i<indexes.length;i++){node.nodeIndex=indexes[i];ids[i]=node.id();selfSizes[i]=node.selfSize();}
this._aggregatesForDiff[className]={indexes:indexes,ids:ids,selfSizes:selfSizes};}
return this._aggregatesForDiff;},isUserRoot:function(node)
{return true;},forEachRoot:function(action,userRootsOnly)
{for(var iter=this.rootNode().edges();iter.hasNext();iter.next()){var node=iter.edge.node();if(!userRootsOnly||this.isUserRoot(node))
action(node);}},calculateDistances:function(filter)
{var nodeCount=this.nodeCount;var distances=this._nodeDistances;var noDistance=this._noDistance;for(var i=0;i<nodeCount;++i)
distances[i]=noDistance;var nodesToVisit=new Uint32Array(this.nodeCount);var nodesToVisitLength=0;function enqueueNode(distance,node)
{var ordinal=node.ordinal();if(distances[ordinal]!==noDistance)
return;distances[ordinal]=distance;nodesToVisit[nodesToVisitLength++]=node.nodeIndex;}
this.forEachRoot(enqueueNode.bind(null,1),true);this._bfs(nodesToVisit,nodesToVisitLength,distances,filter);nodesToVisitLength=0;this.forEachRoot(enqueueNode.bind(null,WebInspector.HeapSnapshotCommon.baseSystemDistance),false);this._bfs(nodesToVisit,nodesToVisitLength,distances,filter);},_bfs:function(nodesToVisit,nodesToVisitLength,distances,filter)
{var edgeFieldsCount=this._edgeFieldsCount;var nodeFieldCount=this._nodeFieldCount;var containmentEdges=this.containmentEdges;var firstEdgeIndexes=this._firstEdgeIndexes;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeTypeOffset=this._edgeTypeOffset;var nodeCount=this.nodeCount;var edgeWeakType=this._edgeWeakType;var noDistance=this._noDistance;var index=0;var edge=this.createEdge(0);var node=this.createNode(0);while(index<nodesToVisitLength){var nodeIndex=nodesToVisit[index++];var nodeOrdinal=nodeIndex/nodeFieldCount;var distance=distances[nodeOrdinal]+1;var firstEdgeIndex=firstEdgeIndexes[nodeOrdinal];var edgesEnd=firstEdgeIndexes[nodeOrdinal+1];node.nodeIndex=nodeIndex;for(var edgeIndex=firstEdgeIndex;edgeIndex<edgesEnd;edgeIndex+=edgeFieldsCount){var edgeType=containmentEdges[edgeIndex+edgeTypeOffset];if(edgeType===edgeWeakType)
continue;var childNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];var childNodeOrdinal=childNodeIndex/nodeFieldCount;if(distances[childNodeOrdinal]!==noDistance)
continue;edge.edgeIndex=edgeIndex;if(filter&&!filter(node,edge))
continue;distances[childNodeOrdinal]=distance;nodesToVisit[nodesToVisitLength++]=childNodeIndex;}}
if(nodesToVisitLength>nodeCount)
throw new Error("BFS failed. Nodes to visit ("+nodesToVisitLength+") is more than nodes count ("+nodeCount+")");},_buildAggregates:function(filter)
{var aggregates={};var aggregatesByClassName={};var classIndexes=[];var nodes=this.nodes;var mapAndFlag=this.userObjectsMapAndFlag();var flags=mapAndFlag?mapAndFlag.map:null;var flag=mapAndFlag?mapAndFlag.flag:0;var nodesLength=nodes.length;var nodeNativeType=this._nodeNativeType;var nodeFieldCount=this._nodeFieldCount;var selfSizeOffset=this._nodeSelfSizeOffset;var nodeTypeOffset=this._nodeTypeOffset;var node=this.rootNode();var nodeDistances=this._nodeDistances;for(var nodeIndex=0;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){var nodeOrdinal=nodeIndex/nodeFieldCount;if(flags&&!(flags[nodeOrdinal]&flag))
continue;node.nodeIndex=nodeIndex;if(filter&&!filter(node))
continue;var selfSize=nodes[nodeIndex+selfSizeOffset];if(!selfSize&&nodes[nodeIndex+nodeTypeOffset]!==nodeNativeType)
continue;var classIndex=node.classIndex();if(!(classIndex in aggregates)){var nodeType=node.type();var nameMatters=nodeType==="object"||nodeType==="native";var value={count:1,distance:nodeDistances[nodeOrdinal],self:selfSize,maxRet:0,type:nodeType,name:nameMatters?node.name():null,idxs:[nodeIndex]};aggregates[classIndex]=value;classIndexes.push(classIndex);aggregatesByClassName[node.className()]=value;}else{var clss=aggregates[classIndex];clss.distance=Math.min(clss.distance,nodeDistances[nodeOrdinal]);++clss.count;clss.self+=selfSize;clss.idxs.push(nodeIndex);}}
for(var i=0,l=classIndexes.length;i<l;++i){var classIndex=classIndexes[i];aggregates[classIndex].idxs=aggregates[classIndex].idxs.slice();}
return{aggregatesByClassName:aggregatesByClassName,aggregatesByClassIndex:aggregates};},_calculateClassesRetainedSize:function(aggregates,filter)
{var rootNodeIndex=this._rootNodeIndex;var node=this.createNode(rootNodeIndex);var list=[rootNodeIndex];var sizes=[-1];var classes=[];var seenClassNameIndexes={};var nodeFieldCount=this._nodeFieldCount;var nodeTypeOffset=this._nodeTypeOffset;var nodeNativeType=this._nodeNativeType;var dominatedNodes=this._dominatedNodes;var nodes=this.nodes;var mapAndFlag=this.userObjectsMapAndFlag();var flags=mapAndFlag?mapAndFlag.map:null;var flag=mapAndFlag?mapAndFlag.flag:0;var firstDominatedNodeIndex=this._firstDominatedNodeIndex;while(list.length){var nodeIndex=list.pop();node.nodeIndex=nodeIndex;var classIndex=node.classIndex();var seen=!!seenClassNameIndexes[classIndex];var nodeOrdinal=nodeIndex/nodeFieldCount;var dominatedIndexFrom=firstDominatedNodeIndex[nodeOrdinal];var dominatedIndexTo=firstDominatedNodeIndex[nodeOrdinal+1];if(!seen&&(!flags||(flags[nodeOrdinal]&flag))&&(!filter||filter(node))&&(node.selfSize()||nodes[nodeIndex+nodeTypeOffset]===nodeNativeType)){aggregates[classIndex].maxRet+=node.retainedSize();if(dominatedIndexFrom!==dominatedIndexTo){seenClassNameIndexes[classIndex]=true;sizes.push(list.length);classes.push(classIndex);}}
for(var i=dominatedIndexFrom;i<dominatedIndexTo;i++)
list.push(dominatedNodes[i]);var l=list.length;while(sizes[sizes.length-1]===l){sizes.pop();classIndex=classes.pop();seenClassNameIndexes[classIndex]=false;}}},_sortAggregateIndexes:function(aggregates)
{var nodeA=this.createNode();var nodeB=this.createNode();for(var clss in aggregates)
aggregates[clss].idxs.sort(function(idxA,idxB){nodeA.nodeIndex=idxA;nodeB.nodeIndex=idxB;return nodeA.id()<nodeB.id()?-1:1;});},_buildPostOrderIndex:function()
{var nodeFieldCount=this._nodeFieldCount;var nodeCount=this.nodeCount;var rootNodeOrdinal=this._rootNodeIndex/nodeFieldCount;var edgeFieldsCount=this._edgeFieldsCount;var edgeTypeOffset=this._edgeTypeOffset;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeShortcutType=this._edgeShortcutType;var edgeWeakType=this._edgeWeakType;var firstEdgeIndexes=this._firstEdgeIndexes;var containmentEdges=this.containmentEdges;var mapAndFlag=this.userObjectsMapAndFlag();var flags=mapAndFlag?mapAndFlag.map:null;var flag=mapAndFlag?mapAndFlag.flag:0;var stackNodes=new Uint32Array(nodeCount);var stackCurrentEdge=new Uint32Array(nodeCount);var postOrderIndex2NodeOrdinal=new Uint32Array(nodeCount);var nodeOrdinal2PostOrderIndex=new Uint32Array(nodeCount);var visited=new Uint8Array(nodeCount);var postOrderIndex=0;var stackTop=0;stackNodes[0]=rootNodeOrdinal;stackCurrentEdge[0]=firstEdgeIndexes[rootNodeOrdinal];visited[rootNodeOrdinal]=1;var iteration=0;while(true){++iteration;while(stackTop>=0){var nodeOrdinal=stackNodes[stackTop];var edgeIndex=stackCurrentEdge[stackTop];var edgesEnd=firstEdgeIndexes[nodeOrdinal+1];if(edgeIndex<edgesEnd){stackCurrentEdge[stackTop]+=edgeFieldsCount;var edgeType=containmentEdges[edgeIndex+edgeTypeOffset];if(edgeType===edgeWeakType||edgeType===edgeShortcutType)
continue;var childNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];var childNodeOrdinal=childNodeIndex/nodeFieldCount;if(visited[childNodeOrdinal])
continue;var nodeFlag=!flags||(flags[nodeOrdinal]&flag);var childNodeFlag=!flags||(flags[childNodeOrdinal]&flag);if(nodeOrdinal!==rootNodeOrdinal&&childNodeFlag&&!nodeFlag)
continue;++stackTop;stackNodes[stackTop]=childNodeOrdinal;stackCurrentEdge[stackTop]=firstEdgeIndexes[childNodeOrdinal];visited[childNodeOrdinal]=1;}else{nodeOrdinal2PostOrderIndex[nodeOrdinal]=postOrderIndex;postOrderIndex2NodeOrdinal[postOrderIndex++]=nodeOrdinal;--stackTop;}}
if(postOrderIndex===nodeCount||iteration>1)
break;var errors=new WebInspector.HeapSnapshotProblemReport("Heap snapshot: "+(nodeCount-postOrderIndex)+" nodes are unreachable from the root. Following nodes have only weak retainers:");var dumpNode=this.rootNode();--postOrderIndex;stackTop=0;stackNodes[0]=rootNodeOrdinal;stackCurrentEdge[0]=firstEdgeIndexes[rootNodeOrdinal+1];for(var i=0;i<nodeCount;++i){if(!visited[i]){dumpNode.nodeIndex=i*nodeFieldCount;if(this._hasOnlyWeakRetainers(i)){stackNodes[++stackTop]=i;stackCurrentEdge[stackTop]=firstEdgeIndexes[i];visited[i]=1;errors.addError(dumpNode.name()+" @"+dumpNode.id());}}}
console.warn(errors.toString());}
if(postOrderIndex!==nodeCount){var errors=new WebInspector.HeapSnapshotProblemReport("Still found "+(nodeCount-postOrderIndex)+" unreachable nodes in heap snapshot:");var dumpNode=this.rootNode();--postOrderIndex;for(var i=0;i<nodeCount;++i){if(visited[i])
continue;dumpNode.nodeIndex=i*nodeFieldCount;errors.addError(dumpNode.name()+" @"+dumpNode.id());nodeOrdinal2PostOrderIndex[i]=postOrderIndex;postOrderIndex2NodeOrdinal[postOrderIndex++]=i;}
nodeOrdinal2PostOrderIndex[rootNodeOrdinal]=postOrderIndex;postOrderIndex2NodeOrdinal[postOrderIndex++]=rootNodeOrdinal;console.warn(errors.toString());}
return{postOrderIndex2NodeOrdinal:postOrderIndex2NodeOrdinal,nodeOrdinal2PostOrderIndex:nodeOrdinal2PostOrderIndex};},_hasOnlyWeakRetainers:function(nodeOrdinal)
{var edgeTypeOffset=this._edgeTypeOffset;var edgeWeakType=this._edgeWeakType;var edgeShortcutType=this._edgeShortcutType;var containmentEdges=this.containmentEdges;var retainingEdges=this._retainingEdges;var beginRetainerIndex=this._firstRetainerIndex[nodeOrdinal];var endRetainerIndex=this._firstRetainerIndex[nodeOrdinal+1];for(var retainerIndex=beginRetainerIndex;retainerIndex<endRetainerIndex;++retainerIndex){var retainerEdgeIndex=retainingEdges[retainerIndex];var retainerEdgeType=containmentEdges[retainerEdgeIndex+edgeTypeOffset];if(retainerEdgeType!==edgeWeakType&&retainerEdgeType!==edgeShortcutType)
return false;}
return true;},_buildDominatorTree:function(postOrderIndex2NodeOrdinal,nodeOrdinal2PostOrderIndex)
{var nodeFieldCount=this._nodeFieldCount;var firstRetainerIndex=this._firstRetainerIndex;var retainingNodes=this._retainingNodes;var retainingEdges=this._retainingEdges;var edgeFieldsCount=this._edgeFieldsCount;var edgeTypeOffset=this._edgeTypeOffset;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeShortcutType=this._edgeShortcutType;var edgeWeakType=this._edgeWeakType;var firstEdgeIndexes=this._firstEdgeIndexes;var containmentEdges=this.containmentEdges;var rootNodeIndex=this._rootNodeIndex;var mapAndFlag=this.userObjectsMapAndFlag();var flags=mapAndFlag?mapAndFlag.map:null;var flag=mapAndFlag?mapAndFlag.flag:0;var nodesCount=postOrderIndex2NodeOrdinal.length;var rootPostOrderedIndex=nodesCount-1;var noEntry=nodesCount;var dominators=new Uint32Array(nodesCount);for(var i=0;i<rootPostOrderedIndex;++i)
dominators[i]=noEntry;dominators[rootPostOrderedIndex]=rootPostOrderedIndex;var affected=new Uint8Array(nodesCount);var nodeOrdinal;{nodeOrdinal=this._rootNodeIndex/nodeFieldCount;var endEdgeIndex=firstEdgeIndexes[nodeOrdinal+1];for(var edgeIndex=firstEdgeIndexes[nodeOrdinal];edgeIndex<endEdgeIndex;edgeIndex+=edgeFieldsCount){var edgeType=containmentEdges[edgeIndex+edgeTypeOffset];if(edgeType===edgeWeakType||edgeType===edgeShortcutType)
continue;var childNodeOrdinal=containmentEdges[edgeIndex+edgeToNodeOffset]/nodeFieldCount;affected[nodeOrdinal2PostOrderIndex[childNodeOrdinal]]=1;}}
var changed=true;while(changed){changed=false;for(var postOrderIndex=rootPostOrderedIndex-1;postOrderIndex>=0;--postOrderIndex){if(affected[postOrderIndex]===0)
continue;affected[postOrderIndex]=0;if(dominators[postOrderIndex]===rootPostOrderedIndex)
continue;nodeOrdinal=postOrderIndex2NodeOrdinal[postOrderIndex];var nodeFlag=!flags||(flags[nodeOrdinal]&flag);var newDominatorIndex=noEntry;var beginRetainerIndex=firstRetainerIndex[nodeOrdinal];var endRetainerIndex=firstRetainerIndex[nodeOrdinal+1];var orphanNode=true;for(var retainerIndex=beginRetainerIndex;retainerIndex<endRetainerIndex;++retainerIndex){var retainerEdgeIndex=retainingEdges[retainerIndex];var retainerEdgeType=containmentEdges[retainerEdgeIndex+edgeTypeOffset];if(retainerEdgeType===edgeWeakType||retainerEdgeType===edgeShortcutType)
continue;orphanNode=false;var retainerNodeIndex=retainingNodes[retainerIndex];var retainerNodeOrdinal=retainerNodeIndex/nodeFieldCount;var retainerNodeFlag=!flags||(flags[retainerNodeOrdinal]&flag);if(retainerNodeIndex!==rootNodeIndex&&nodeFlag&&!retainerNodeFlag)
continue;var retanerPostOrderIndex=nodeOrdinal2PostOrderIndex[retainerNodeOrdinal];if(dominators[retanerPostOrderIndex]!==noEntry){if(newDominatorIndex===noEntry)
newDominatorIndex=retanerPostOrderIndex;else{while(retanerPostOrderIndex!==newDominatorIndex){while(retanerPostOrderIndex<newDominatorIndex)
retanerPostOrderIndex=dominators[retanerPostOrderIndex];while(newDominatorIndex<retanerPostOrderIndex)
newDominatorIndex=dominators[newDominatorIndex];}}
if(newDominatorIndex===rootPostOrderedIndex)
break;}}
if(orphanNode)
newDominatorIndex=rootPostOrderedIndex;if(newDominatorIndex!==noEntry&&dominators[postOrderIndex]!==newDominatorIndex){dominators[postOrderIndex]=newDominatorIndex;changed=true;nodeOrdinal=postOrderIndex2NodeOrdinal[postOrderIndex];var beginEdgeToNodeFieldIndex=firstEdgeIndexes[nodeOrdinal]+edgeToNodeOffset;var endEdgeToNodeFieldIndex=firstEdgeIndexes[nodeOrdinal+1];for(var toNodeFieldIndex=beginEdgeToNodeFieldIndex;toNodeFieldIndex<endEdgeToNodeFieldIndex;toNodeFieldIndex+=edgeFieldsCount){var childNodeOrdinal=containmentEdges[toNodeFieldIndex]/nodeFieldCount;affected[nodeOrdinal2PostOrderIndex[childNodeOrdinal]]=1;}}}}
var dominatorsTree=new Uint32Array(nodesCount);for(var postOrderIndex=0,l=dominators.length;postOrderIndex<l;++postOrderIndex){nodeOrdinal=postOrderIndex2NodeOrdinal[postOrderIndex];dominatorsTree[nodeOrdinal]=postOrderIndex2NodeOrdinal[dominators[postOrderIndex]];}
return dominatorsTree;},_calculateRetainedSizes:function(postOrderIndex2NodeOrdinal)
{var nodeCount=this.nodeCount;var nodes=this.nodes;var nodeSelfSizeOffset=this._nodeSelfSizeOffset;var nodeFieldCount=this._nodeFieldCount;var dominatorsTree=this._dominatorsTree;var retainedSizes=this._retainedSizes;for(var nodeOrdinal=0;nodeOrdinal<nodeCount;++nodeOrdinal)
retainedSizes[nodeOrdinal]=nodes[nodeOrdinal*nodeFieldCount+nodeSelfSizeOffset];for(var postOrderIndex=0;postOrderIndex<nodeCount-1;++postOrderIndex){var nodeOrdinal=postOrderIndex2NodeOrdinal[postOrderIndex];var dominatorOrdinal=dominatorsTree[nodeOrdinal];retainedSizes[dominatorOrdinal]+=retainedSizes[nodeOrdinal];}},_buildDominatedNodes:function()
{var indexArray=this._firstDominatedNodeIndex;var dominatedNodes=this._dominatedNodes;var nodeFieldCount=this._nodeFieldCount;var dominatorsTree=this._dominatorsTree;var fromNodeOrdinal=0;var toNodeOrdinal=this.nodeCount;var rootNodeOrdinal=this._rootNodeIndex/nodeFieldCount;if(rootNodeOrdinal===fromNodeOrdinal)
fromNodeOrdinal=1;else if(rootNodeOrdinal===toNodeOrdinal-1)
toNodeOrdinal=toNodeOrdinal-1;else
throw new Error("Root node is expected to be either first or last");for(var nodeOrdinal=fromNodeOrdinal;nodeOrdinal<toNodeOrdinal;++nodeOrdinal)
++indexArray[dominatorsTree[nodeOrdinal]];var firstDominatedNodeIndex=0;for(var i=0,l=this.nodeCount;i<l;++i){var dominatedCount=dominatedNodes[firstDominatedNodeIndex]=indexArray[i];indexArray[i]=firstDominatedNodeIndex;firstDominatedNodeIndex+=dominatedCount;}
indexArray[this.nodeCount]=dominatedNodes.length;for(var nodeOrdinal=fromNodeOrdinal;nodeOrdinal<toNodeOrdinal;++nodeOrdinal){var dominatorOrdinal=dominatorsTree[nodeOrdinal];var dominatedRefIndex=indexArray[dominatorOrdinal];dominatedRefIndex+=(--dominatedNodes[dominatedRefIndex]);dominatedNodes[dominatedRefIndex]=nodeOrdinal*nodeFieldCount;}},_buildSamples:function()
{var samples=this._rawSamples;if(!samples||!samples.length)
return;var sampleCount=samples.length/2;var sizeForRange=new Array(sampleCount);var timestamps=new Array(sampleCount);var lastAssignedIds=new Array(sampleCount);var timestampOffset=this._metaNode.sample_fields.indexOf("timestamp_us");var lastAssignedIdOffset=this._metaNode.sample_fields.indexOf("last_assigned_id");for(var i=0;i<sampleCount;i++){sizeForRange[i]=0;timestamps[i]=(samples[2*i+timestampOffset])/1000;lastAssignedIds[i]=samples[2*i+lastAssignedIdOffset];}
var nodes=this.nodes;var nodesLength=nodes.length;var nodeFieldCount=this._nodeFieldCount;var node=this.rootNode();for(var nodeIndex=0;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){node.nodeIndex=nodeIndex;var nodeId=node.id();if(nodeId%2===0)
continue;var rangeIndex=lastAssignedIds.lowerBound(nodeId);if(rangeIndex===sampleCount){continue;}
sizeForRange[rangeIndex]+=node.selfSize();}
this._samples=new WebInspector.HeapSnapshotCommon.Samples(timestamps,lastAssignedIds,sizeForRange);},getSamples:function()
{return this._samples;},_calculateFlags:function()
{throw new Error("Not implemented");},_calculateStatistics:function()
{throw new Error("Not implemented");},userObjectsMapAndFlag:function()
{throw new Error("Not implemented");},calculateSnapshotDiff:function(baseSnapshotId,baseSnapshotAggregates)
{var snapshotDiff=this._snapshotDiffs[baseSnapshotId];if(snapshotDiff)
return snapshotDiff;snapshotDiff={};var aggregates=this.aggregates(true,"allObjects");for(var className in baseSnapshotAggregates){var baseAggregate=baseSnapshotAggregates[className];var diff=this._calculateDiffForClass(baseAggregate,aggregates[className]);if(diff)
snapshotDiff[className]=diff;}
var emptyBaseAggregate=new WebInspector.HeapSnapshotCommon.AggregateForDiff();for(var className in aggregates){if(className in baseSnapshotAggregates)
continue;snapshotDiff[className]=this._calculateDiffForClass(emptyBaseAggregate,aggregates[className]);}
this._snapshotDiffs[baseSnapshotId]=snapshotDiff;return snapshotDiff;},_calculateDiffForClass:function(baseAggregate,aggregate)
{var baseIds=baseAggregate.ids;var baseIndexes=baseAggregate.indexes;var baseSelfSizes=baseAggregate.selfSizes;var indexes=aggregate?aggregate.idxs:[];var i=0,l=baseIds.length;var j=0,m=indexes.length;var diff=new WebInspector.HeapSnapshotCommon.Diff();var nodeB=this.createNode(indexes[j]);while(i<l&&j<m){var nodeAId=baseIds[i];if(nodeAId<nodeB.id()){diff.deletedIndexes.push(baseIndexes[i]);diff.removedCount++;diff.removedSize+=baseSelfSizes[i];++i;}else if(nodeAId>nodeB.id()){diff.addedIndexes.push(indexes[j]);diff.addedCount++;diff.addedSize+=nodeB.selfSize();nodeB.nodeIndex=indexes[++j];}else{++i;nodeB.nodeIndex=indexes[++j];}}
while(i<l){diff.deletedIndexes.push(baseIndexes[i]);diff.removedCount++;diff.removedSize+=baseSelfSizes[i];++i;}
while(j<m){diff.addedIndexes.push(indexes[j]);diff.addedCount++;diff.addedSize+=nodeB.selfSize();nodeB.nodeIndex=indexes[++j];}
diff.countDelta=diff.addedCount-diff.removedCount;diff.sizeDelta=diff.addedSize-diff.removedSize;if(!diff.addedCount&&!diff.removedCount)
return null;return diff;},_nodeForSnapshotObjectId:function(snapshotObjectId)
{for(var it=this._allNodes();it.hasNext();it.next()){if(it.node.id()===snapshotObjectId)
return it.node;}
return null;},nodeClassName:function(snapshotObjectId)
{var node=this._nodeForSnapshotObjectId(snapshotObjectId);if(node)
return node.className();return null;},idsOfObjectsWithName:function(name)
{var ids=[];for(var it=this._allNodes();it.hasNext();it.next()){if(it.item().name()===name)
ids.push(it.item().id());}
return ids;},createEdgesProvider:function(nodeIndex)
{var node=this.createNode(nodeIndex);var filter=this.containmentEdgesFilter();var indexProvider=new WebInspector.HeapSnapshotEdgeIndexProvider(this);return new WebInspector.HeapSnapshotEdgesProvider(this,filter,node.edges(),indexProvider);},createEdgesProviderForTest:function(nodeIndex,filter)
{var node=this.createNode(nodeIndex);var indexProvider=new WebInspector.HeapSnapshotEdgeIndexProvider(this);return new WebInspector.HeapSnapshotEdgesProvider(this,filter,node.edges(),indexProvider);},retainingEdgesFilter:function()
{return null;},containmentEdgesFilter:function()
{return null;},createRetainingEdgesProvider:function(nodeIndex)
{var node=this.createNode(nodeIndex);var filter=this.retainingEdgesFilter();var indexProvider=new WebInspector.HeapSnapshotRetainerEdgeIndexProvider(this);return new WebInspector.HeapSnapshotEdgesProvider(this,filter,node.retainers(),indexProvider);},createAddedNodesProvider:function(baseSnapshotId,className)
{var snapshotDiff=this._snapshotDiffs[baseSnapshotId];var diffForClass=snapshotDiff[className];return new WebInspector.HeapSnapshotNodesProvider(this,null,diffForClass.addedIndexes);},createDeletedNodesProvider:function(nodeIndexes)
{return new WebInspector.HeapSnapshotNodesProvider(this,null,nodeIndexes);},classNodesFilter:function()
{return null;},createNodesProviderForClass:function(className,nodeFilter)
{return new WebInspector.HeapSnapshotNodesProvider(this,this.classNodesFilter(),this.aggregatesWithFilter(nodeFilter)[className].idxs);},_maxJsNodeId:function()
{var nodeFieldCount=this._nodeFieldCount;var nodes=this.nodes;var nodesLength=nodes.length;var id=0;for(var nodeIndex=this._nodeIdOffset;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){var nextId=nodes[nodeIndex];if(nextId%2===0)
continue;if(id<nextId)
id=nextId;}
return id;},updateStaticData:function()
{return new WebInspector.HeapSnapshotCommon.StaticData(this.nodeCount,this._rootNodeIndex,this.totalSize,this._maxJsNodeId());}};WebInspector.HeapSnapshotItemProvider=function(iterator,indexProvider)
{this._iterator=iterator;this._indexProvider=indexProvider;this._isEmpty=!iterator.hasNext();this._iterationOrder=null;this._currentComparator=null;this._sortedPrefixLength=0;this._sortedSuffixLength=0;}
WebInspector.HeapSnapshotItemProvider.prototype={_createIterationOrder:function()
{if(this._iterationOrder)
return;this._iterationOrder=[];for(var iterator=this._iterator;iterator.hasNext();iterator.next())
this._iterationOrder.push(iterator.item().itemIndex());},isEmpty:function()
{return this._isEmpty;},serializeItemsRange:function(begin,end)
{this._createIterationOrder();if(begin>end)
throw new Error("Start position > end position: "+begin+" > "+end);if(end>this._iterationOrder.length)
end=this._iterationOrder.length;if(this._sortedPrefixLength<end&&begin<this._iterationOrder.length-this._sortedSuffixLength){this.sort(this._currentComparator,this._sortedPrefixLength,this._iterationOrder.length-1-this._sortedSuffixLength,begin,end-1);if(begin<=this._sortedPrefixLength)
this._sortedPrefixLength=end;if(end>=this._iterationOrder.length-this._sortedSuffixLength)
this._sortedSuffixLength=this._iterationOrder.length-begin;}
var position=begin;var count=end-begin;var result=new Array(count);for(var i=0;i<count;++i){var itemIndex=this._iterationOrder[position++];var item=this._indexProvider.itemForIndex(itemIndex);result[i]=item.serialize();}
return new WebInspector.HeapSnapshotCommon.ItemsRange(begin,end,this._iterationOrder.length,result);},sortAndRewind:function(comparator)
{this._currentComparator=comparator;this._sortedPrefixLength=0;this._sortedSuffixLength=0;}}
WebInspector.HeapSnapshotEdgesProvider=function(snapshot,filter,edgesIter,indexProvider)
{this.snapshot=snapshot;var iter=filter?new WebInspector.HeapSnapshotFilteredIterator(edgesIter,(filter)):edgesIter;WebInspector.HeapSnapshotItemProvider.call(this,iter,indexProvider);}
WebInspector.HeapSnapshotEdgesProvider.prototype={sort:function(comparator,leftBound,rightBound,windowLeft,windowRight)
{var fieldName1=comparator.fieldName1;var fieldName2=comparator.fieldName2;var ascending1=comparator.ascending1;var ascending2=comparator.ascending2;var edgeA=this._iterator.item().clone();var edgeB=edgeA.clone();var nodeA=this.snapshot.createNode();var nodeB=this.snapshot.createNode();function compareEdgeFieldName(ascending,indexA,indexB)
{edgeA.edgeIndex=indexA;edgeB.edgeIndex=indexB;if(edgeB.name()==="__proto__")return-1;if(edgeA.name()==="__proto__")return 1;var result=edgeA.hasStringName()===edgeB.hasStringName()?(edgeA.name()<edgeB.name()?-1:(edgeA.name()>edgeB.name()?1:0)):(edgeA.hasStringName()?-1:1);return ascending?result:-result;}
function compareNodeField(fieldName,ascending,indexA,indexB)
{edgeA.edgeIndex=indexA;nodeA.nodeIndex=edgeA.nodeIndex();var valueA=nodeA[fieldName]();edgeB.edgeIndex=indexB;nodeB.nodeIndex=edgeB.nodeIndex();var valueB=nodeB[fieldName]();var result=valueA<valueB?-1:(valueA>valueB?1:0);return ascending?result:-result;}
function compareEdgeAndNode(indexA,indexB){var result=compareEdgeFieldName(ascending1,indexA,indexB);if(result===0)
result=compareNodeField(fieldName2,ascending2,indexA,indexB);if(result===0)
return indexA-indexB;return result;}
function compareNodeAndEdge(indexA,indexB){var result=compareNodeField(fieldName1,ascending1,indexA,indexB);if(result===0)
result=compareEdgeFieldName(ascending2,indexA,indexB);if(result===0)
return indexA-indexB;return result;}
function compareNodeAndNode(indexA,indexB){var result=compareNodeField(fieldName1,ascending1,indexA,indexB);if(result===0)
result=compareNodeField(fieldName2,ascending2,indexA,indexB);if(result===0)
return indexA-indexB;return result;}
if(fieldName1==="!edgeName")
this._iterationOrder.sortRange(compareEdgeAndNode,leftBound,rightBound,windowLeft,windowRight);else if(fieldName2==="!edgeName")
this._iterationOrder.sortRange(compareNodeAndEdge,leftBound,rightBound,windowLeft,windowRight);else
this._iterationOrder.sortRange(compareNodeAndNode,leftBound,rightBound,windowLeft,windowRight);},__proto__:WebInspector.HeapSnapshotItemProvider.prototype}
WebInspector.HeapSnapshotNodesProvider=function(snapshot,filter,nodeIndexes)
{this.snapshot=snapshot;var indexProvider=new WebInspector.HeapSnapshotNodeIndexProvider(snapshot);var it=new WebInspector.HeapSnapshotIndexRangeIterator(indexProvider,nodeIndexes);if(filter)
it=new WebInspector.HeapSnapshotFilteredIterator(it,(filter));WebInspector.HeapSnapshotItemProvider.call(this,it,indexProvider);}
WebInspector.HeapSnapshotNodesProvider.prototype={nodePosition:function(snapshotObjectId)
{this._createIterationOrder();var node=this.snapshot.createNode();for(var i=0;i<this._iterationOrder.length;i++){node.nodeIndex=this._iterationOrder[i];if(node.id()===snapshotObjectId)
break;}
if(i===this._iterationOrder.length)
return-1;var targetNodeIndex=this._iterationOrder[i];var smallerCount=0;var compare=this._buildCompareFunction(this._currentComparator);for(var i=0;i<this._iterationOrder.length;i++){if(compare(this._iterationOrder[i],targetNodeIndex)<0)
++smallerCount;}
return smallerCount;},_buildCompareFunction:function(comparator)
{var nodeA=this.snapshot.createNode();var nodeB=this.snapshot.createNode();var fieldAccessor1=nodeA[comparator.fieldName1];var fieldAccessor2=nodeA[comparator.fieldName2];var ascending1=comparator.ascending1?1:-1;var ascending2=comparator.ascending2?1:-1;function sortByNodeField(fieldAccessor,ascending)
{var valueA=fieldAccessor.call(nodeA);var valueB=fieldAccessor.call(nodeB);return valueA<valueB?-ascending:(valueA>valueB?ascending:0);}
function sortByComparator(indexA,indexB)
{nodeA.nodeIndex=indexA;nodeB.nodeIndex=indexB;var result=sortByNodeField(fieldAccessor1,ascending1);if(result===0)
result=sortByNodeField(fieldAccessor2,ascending2);return result||indexA-indexB;}
return sortByComparator;},sort:function(comparator,leftBound,rightBound,windowLeft,windowRight)
{this._iterationOrder.sortRange(this._buildCompareFunction(comparator),leftBound,rightBound,windowLeft,windowRight);},__proto__:WebInspector.HeapSnapshotItemProvider.prototype};WebInspector.HeapSnapshotLoader=function(dispatcher)
{this._reset();this._progress=new WebInspector.HeapSnapshotProgress(dispatcher);}
WebInspector.HeapSnapshotLoader.prototype={dispose:function()
{this._reset();},_reset:function()
{this._json="";this._state="find-snapshot-info";this._snapshot={};},close:function()
{if(this._json)
this._parseStringsArray();},buildSnapshot:function(showHiddenData)
{this._progress.updateStatus("Processing snapshot\u2026");var result=new WebInspector.JSHeapSnapshot(this._snapshot,this._progress,showHiddenData);this._reset();return result;},_parseUintArray:function()
{var index=0;var char0="0".charCodeAt(0),char9="9".charCodeAt(0),closingBracket="]".charCodeAt(0);var length=this._json.length;while(true){while(index<length){var code=this._json.charCodeAt(index);if(char0<=code&&code<=char9)
break;else if(code===closingBracket){this._json=this._json.slice(index+1);return false;}
++index;}
if(index===length){this._json="";return true;}
var nextNumber=0;var startIndex=index;while(index<length){var code=this._json.charCodeAt(index);if(char0>code||code>char9)
break;nextNumber*=10;nextNumber+=(code-char0);++index;}
if(index===length){this._json=this._json.slice(startIndex);return true;}
this._array[this._arrayIndex++]=nextNumber;}},_parseStringsArray:function()
{this._progress.updateStatus("Parsing strings\u2026");var closingBracketIndex=this._json.lastIndexOf("]");if(closingBracketIndex===-1)
throw new Error("Incomplete JSON");this._json=this._json.slice(0,closingBracketIndex+1);this._snapshot.strings=JSON.parse(this._json);},write:function(chunk)
{if(this._json!==null)
this._json+=chunk;while(true){switch(this._state){case"find-snapshot-info":{var snapshotToken="\"snapshot\"";var snapshotTokenIndex=this._json.indexOf(snapshotToken);if(snapshotTokenIndex===-1)
throw new Error("Snapshot token not found");var json=this._json.slice(snapshotTokenIndex+snapshotToken.length+1);this._state="parse-snapshot-info";this._progress.updateStatus("Loading snapshot info\u2026");this._json=null;this._jsonTokenizer=new WebInspector.TextUtils.BalancedJSONTokenizer(this._writeBalancedJSON.bind(this));chunk=json;}
case"parse-snapshot-info":{this._jsonTokenizer.write(chunk);if(this._jsonTokenizer)
return;break;}
case"find-nodes":{var nodesToken="\"nodes\"";var nodesTokenIndex=this._json.indexOf(nodesToken);if(nodesTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",nodesTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex+1);var node_fields_count=this._snapshot.snapshot.meta.node_fields.length;var nodes_length=this._snapshot.snapshot.node_count*node_fields_count;this._array=new Uint32Array(nodes_length);this._arrayIndex=0;this._state="parse-nodes";break;}
case"parse-nodes":{var hasMoreData=this._parseUintArray();this._progress.updateProgress("Loading nodes\u2026 %d%%",this._arrayIndex,this._array.length);if(hasMoreData)
return;this._snapshot.nodes=this._array;this._state="find-edges";this._array=null;break;}
case"find-edges":{var edgesToken="\"edges\"";var edgesTokenIndex=this._json.indexOf(edgesToken);if(edgesTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",edgesTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex+1);var edge_fields_count=this._snapshot.snapshot.meta.edge_fields.length;var edges_length=this._snapshot.snapshot.edge_count*edge_fields_count;this._array=new Uint32Array(edges_length);this._arrayIndex=0;this._state="parse-edges";break;}
case"parse-edges":{var hasMoreData=this._parseUintArray();this._progress.updateProgress("Loading edges\u2026 %d%%",this._arrayIndex,this._array.length);if(hasMoreData)
return;this._snapshot.edges=this._array;this._array=null;if(this._snapshot.snapshot.trace_function_count){this._state="find-trace-function-infos";this._progress.updateStatus("Loading allocation traces\u2026");}else if(this._snapshot.snapshot.meta.sample_fields){this._state="find-samples";this._progress.updateStatus("Loading samples\u2026");}else{this._state="find-strings";}
break;}
case"find-trace-function-infos":{var tracesToken="\"trace_function_infos\"";var tracesTokenIndex=this._json.indexOf(tracesToken);if(tracesTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",tracesTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex+1);var trace_function_info_field_count=this._snapshot.snapshot.meta.trace_function_info_fields.length;var trace_function_info_length=this._snapshot.snapshot.trace_function_count*trace_function_info_field_count;this._array=new Uint32Array(trace_function_info_length);this._arrayIndex=0;this._state="parse-trace-function-infos";break;}
case"parse-trace-function-infos":{if(this._parseUintArray())
return;this._snapshot.trace_function_infos=this._array;this._array=null;this._state="find-trace-tree";break;}
case"find-trace-tree":{var tracesToken="\"trace_tree\"";var tracesTokenIndex=this._json.indexOf(tracesToken);if(tracesTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",tracesTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex);this._state="parse-trace-tree";break;}
case"parse-trace-tree":{var nextToken=this._snapshot.snapshot.meta.sample_fields?"\"samples\"":"\"strings\"";var nextTokenIndex=this._json.indexOf(nextToken);if(nextTokenIndex===-1)
return;var bracketIndex=this._json.lastIndexOf("]",nextTokenIndex);this._snapshot.trace_tree=JSON.parse(this._json.substring(0,bracketIndex+1));this._json=this._json.slice(bracketIndex+1);if(this._snapshot.snapshot.meta.sample_fields){this._state="find-samples";this._progress.updateStatus("Loading samples\u2026");}else{this._state="find-strings";this._progress.updateStatus("Loading strings\u2026");}
break;}
case"find-samples":{var samplesToken="\"samples\"";var samplesTokenIndex=this._json.indexOf(samplesToken);if(samplesTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",samplesTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex+1);this._array=[];this._arrayIndex=0;this._state="parse-samples";break;}
case"parse-samples":{if(this._parseUintArray())
return;this._snapshot.samples=this._array;this._array=null;this._state="find-strings";this._progress.updateStatus("Loading strings\u2026");break;}
case"find-strings":{var stringsToken="\"strings\"";var stringsTokenIndex=this._json.indexOf(stringsToken);if(stringsTokenIndex===-1)
return;var bracketIndex=this._json.indexOf("[",stringsTokenIndex);if(bracketIndex===-1)
return;this._json=this._json.slice(bracketIndex);this._state="accumulate-strings";break;}
case"accumulate-strings":return;}}},_writeBalancedJSON:function(data)
{this._json=this._jsonTokenizer.remainder();this._jsonTokenizer=null;this._state="find-nodes";this._snapshot.snapshot=(JSON.parse(data));}};WebInspector.HeapSnapshotWorkerDispatcher=function(globalObject,postMessage)
{this._objects=[];this._global=globalObject;this._postMessage=postMessage;}
WebInspector.HeapSnapshotWorkerDispatcher.prototype={_findFunction:function(name)
{var path=name.split(".");var result=this._global;for(var i=0;i<path.length;++i)
result=result[path[i]];return result;},sendEvent:function(name,data)
{this._postMessage({eventName:name,data:data});},dispatchMessage:function(event)
{var data=(event.data);var response={callId:data.callId};try{switch(data.disposition){case"create":{var constructorFunction=this._findFunction(data.methodName);this._objects[data.objectId]=new constructorFunction(this);break;}
case"dispose":{delete this._objects[data.objectId];break;}
case"getter":{var object=this._objects[data.objectId];var result=object[data.methodName];response.result=result;break;}
case"factory":{var object=this._objects[data.objectId];var result=object[data.methodName].apply(object,data.methodArguments);if(result)
this._objects[data.newObjectId]=result;response.result=!!result;break;}
case"method":{var object=this._objects[data.objectId];response.result=object[data.methodName].apply(object,data.methodArguments);break;}
case"evaluateForTest":{try{response.result=eval(data.source);}catch(e){response.result=e.toString();}
break;}}}catch(e){response.error=e.toString();response.errorCallStack=e.stack;if(data.methodName)
response.errorMethodName=data.methodName;}
this._postMessage(response);}};;WebInspector.JSHeapSnapshot=function(profile,progress,showHiddenData)
{this._nodeFlags={canBeQueried:1,detachedDOMTreeNode:2,pageObject:4,visitedMarkerMask:0x0ffff,visitedMarker:0x10000};this._lazyStringCache={};this._showHiddenData=showHiddenData;WebInspector.HeapSnapshot.call(this,profile,progress);}
WebInspector.JSHeapSnapshot.prototype={createNode:function(nodeIndex)
{return new WebInspector.JSHeapSnapshotNode(this,nodeIndex===undefined?-1:nodeIndex);},createEdge:function(edgeIndex)
{return new WebInspector.JSHeapSnapshotEdge(this,edgeIndex);},createRetainingEdge:function(retainerIndex)
{return new WebInspector.JSHeapSnapshotRetainerEdge(this,retainerIndex);},classNodesFilter:function()
{var mapAndFlag=this.userObjectsMapAndFlag();if(!mapAndFlag)
return null;var map=mapAndFlag.map;var flag=mapAndFlag.flag;function filter(node)
{return!!(map[node.ordinal()]&flag);}
return filter;},containmentEdgesFilter:function()
{var showHiddenData=this._showHiddenData;function filter(edge)
{if(edge.isInvisible())
return false;if(showHiddenData)
return true;return!edge.isHidden()&&!edge.node().isHidden();}
return filter;},retainingEdgesFilter:function()
{var containmentEdgesFilter=this.containmentEdgesFilter();function filter(edge)
{return containmentEdgesFilter(edge)&&!edge.node().isRoot()&&!edge.isWeak();}
return filter;},_calculateFlags:function()
{this._flags=new Uint32Array(this.nodeCount);this._markDetachedDOMTreeNodes();this._markQueriableHeapObjects();this._markPageOwnedNodes();},calculateDistances:function()
{function filter(node,edge)
{if(node.isHidden())
return edge.name()!=="sloppy_function_map"||node.rawName()!=="system / NativeContext";if(node.isArray()){if(node.rawName()!=="(map descriptors)")
return true;var index=edge.name();return index<2||(index%3)!==1;}
return true;}
WebInspector.HeapSnapshot.prototype.calculateDistances.call(this,filter);},isUserRoot:function(node)
{return node.isUserRoot()||node.isDocumentDOMTreesRoot();},forEachRoot:function(action,userRootsOnly)
{function getChildNodeByName(node,name)
{for(var iter=node.edges();iter.hasNext();iter.next()){var child=iter.edge.node();if(child.name()===name)
return child;}
return null;}
var visitedNodes={};function doAction(node)
{var ordinal=node.ordinal();if(!visitedNodes[ordinal]){action(node);visitedNodes[ordinal]=true;}}
var gcRoots=getChildNodeByName(this.rootNode(),"(GC roots)");if(!gcRoots)
return;if(userRootsOnly){for(var iter=this.rootNode().edges();iter.hasNext();iter.next()){var node=iter.edge.node();if(this.isUserRoot(node))
doAction(node);}}else{for(var iter=gcRoots.edges();iter.hasNext();iter.next()){var subRoot=iter.edge.node();for(var iter2=subRoot.edges();iter2.hasNext();iter2.next())
doAction(iter2.edge.node());doAction(subRoot);}
for(var iter=this.rootNode().edges();iter.hasNext();iter.next())
doAction(iter.edge.node());}},userObjectsMapAndFlag:function()
{return this._showHiddenData?null:{map:this._flags,flag:this._nodeFlags.pageObject};},_flagsOfNode:function(node)
{return this._flags[node.nodeIndex/this._nodeFieldCount];},_markDetachedDOMTreeNodes:function()
{var flag=this._nodeFlags.detachedDOMTreeNode;var detachedDOMTreesRoot;for(var iter=this.rootNode().edges();iter.hasNext();iter.next()){var node=iter.edge.node();if(node.name()==="(Detached DOM trees)"){detachedDOMTreesRoot=node;break;}}
if(!detachedDOMTreesRoot)
return;var detachedDOMTreeRE=/^Detached DOM tree/;for(var iter=detachedDOMTreesRoot.edges();iter.hasNext();iter.next()){var node=iter.edge.node();if(detachedDOMTreeRE.test(node.className())){for(var edgesIter=node.edges();edgesIter.hasNext();edgesIter.next())
this._flags[edgesIter.edge.node().nodeIndex/this._nodeFieldCount]|=flag;}}},_markQueriableHeapObjects:function()
{var flag=this._nodeFlags.canBeQueried;var hiddenEdgeType=this._edgeHiddenType;var internalEdgeType=this._edgeInternalType;var invisibleEdgeType=this._edgeInvisibleType;var weakEdgeType=this._edgeWeakType;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeTypeOffset=this._edgeTypeOffset;var edgeFieldsCount=this._edgeFieldsCount;var containmentEdges=this.containmentEdges;var nodeFieldCount=this._nodeFieldCount;var firstEdgeIndexes=this._firstEdgeIndexes;var flags=this._flags;var list=[];for(var iter=this.rootNode().edges();iter.hasNext();iter.next()){if(iter.edge.node().isUserRoot())
list.push(iter.edge.node().nodeIndex/nodeFieldCount);}
while(list.length){var nodeOrdinal=list.pop();if(flags[nodeOrdinal]&flag)
continue;flags[nodeOrdinal]|=flag;var beginEdgeIndex=firstEdgeIndexes[nodeOrdinal];var endEdgeIndex=firstEdgeIndexes[nodeOrdinal+1];for(var edgeIndex=beginEdgeIndex;edgeIndex<endEdgeIndex;edgeIndex+=edgeFieldsCount){var childNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];var childNodeOrdinal=childNodeIndex/nodeFieldCount;if(flags[childNodeOrdinal]&flag)
continue;var type=containmentEdges[edgeIndex+edgeTypeOffset];if(type===hiddenEdgeType||type===invisibleEdgeType||type===internalEdgeType||type===weakEdgeType)
continue;list.push(childNodeOrdinal);}}},_markPageOwnedNodes:function()
{var edgeShortcutType=this._edgeShortcutType;var edgeElementType=this._edgeElementType;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeTypeOffset=this._edgeTypeOffset;var edgeFieldsCount=this._edgeFieldsCount;var edgeWeakType=this._edgeWeakType;var firstEdgeIndexes=this._firstEdgeIndexes;var containmentEdges=this.containmentEdges;var nodeFieldCount=this._nodeFieldCount;var nodesCount=this.nodeCount;var flags=this._flags;var flag=this._nodeFlags.pageObject;var visitedMarker=this._nodeFlags.visitedMarker;var visitedMarkerMask=this._nodeFlags.visitedMarkerMask;var markerAndFlag=visitedMarker|flag;var nodesToVisit=new Uint32Array(nodesCount);var nodesToVisitLength=0;var rootNodeOrdinal=this._rootNodeIndex/nodeFieldCount;var node=this.rootNode();for(var edgeIndex=firstEdgeIndexes[rootNodeOrdinal],endEdgeIndex=firstEdgeIndexes[rootNodeOrdinal+1];edgeIndex<endEdgeIndex;edgeIndex+=edgeFieldsCount){var edgeType=containmentEdges[edgeIndex+edgeTypeOffset];var nodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];if(edgeType===edgeElementType){node.nodeIndex=nodeIndex;if(!node.isDocumentDOMTreesRoot())
continue;}else if(edgeType!==edgeShortcutType)
continue;var nodeOrdinal=nodeIndex/nodeFieldCount;nodesToVisit[nodesToVisitLength++]=nodeOrdinal;flags[nodeOrdinal]|=visitedMarker;}
while(nodesToVisitLength){var nodeOrdinal=nodesToVisit[--nodesToVisitLength];flags[nodeOrdinal]|=flag;flags[nodeOrdinal]&=visitedMarkerMask;var beginEdgeIndex=firstEdgeIndexes[nodeOrdinal];var endEdgeIndex=firstEdgeIndexes[nodeOrdinal+1];for(var edgeIndex=beginEdgeIndex;edgeIndex<endEdgeIndex;edgeIndex+=edgeFieldsCount){var childNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];var childNodeOrdinal=childNodeIndex/nodeFieldCount;if(flags[childNodeOrdinal]&markerAndFlag)
continue;var type=containmentEdges[edgeIndex+edgeTypeOffset];if(type===edgeWeakType)
continue;nodesToVisit[nodesToVisitLength++]=childNodeOrdinal;flags[childNodeOrdinal]|=visitedMarker;}}},_calculateStatistics:function()
{var nodeFieldCount=this._nodeFieldCount;var nodes=this.nodes;var nodesLength=nodes.length;var nodeTypeOffset=this._nodeTypeOffset;var nodeSizeOffset=this._nodeSelfSizeOffset;;var nodeNativeType=this._nodeNativeType;var nodeCodeType=this._nodeCodeType;var nodeConsStringType=this._nodeConsStringType;var nodeSlicedStringType=this._nodeSlicedStringType;var distances=this._nodeDistances;var sizeNative=0;var sizeCode=0;var sizeStrings=0;var sizeJSArrays=0;var sizeSystem=0;var node=this.rootNode();for(var nodeIndex=0;nodeIndex<nodesLength;nodeIndex+=nodeFieldCount){var nodeSize=nodes[nodeIndex+nodeSizeOffset];var ordinal=nodeIndex/nodeFieldCount;if(distances[ordinal]>=WebInspector.HeapSnapshotCommon.baseSystemDistance){sizeSystem+=nodeSize;continue;}
var nodeType=nodes[nodeIndex+nodeTypeOffset];node.nodeIndex=nodeIndex;if(nodeType===nodeNativeType)
sizeNative+=nodeSize;else if(nodeType===nodeCodeType)
sizeCode+=nodeSize;else if(nodeType===nodeConsStringType||nodeType===nodeSlicedStringType||node.type()==="string")
sizeStrings+=nodeSize;else if(node.name()==="Array")
sizeJSArrays+=this._calculateArraySize(node);}
this._statistics=new WebInspector.HeapSnapshotCommon.Statistics();this._statistics.total=this.totalSize;this._statistics.v8heap=this.totalSize-sizeNative;this._statistics.native=sizeNative;this._statistics.code=sizeCode;this._statistics.jsArrays=sizeJSArrays;this._statistics.strings=sizeStrings;this._statistics.system=sizeSystem;},_calculateArraySize:function(node)
{var size=node.selfSize();var beginEdgeIndex=node.edgeIndexesStart();var endEdgeIndex=node.edgeIndexesEnd();var containmentEdges=this.containmentEdges;var strings=this.strings;var edgeToNodeOffset=this._edgeToNodeOffset;var edgeTypeOffset=this._edgeTypeOffset;var edgeNameOffset=this._edgeNameOffset;var edgeFieldsCount=this._edgeFieldsCount;var edgeInternalType=this._edgeInternalType;for(var edgeIndex=beginEdgeIndex;edgeIndex<endEdgeIndex;edgeIndex+=edgeFieldsCount){var edgeType=containmentEdges[edgeIndex+edgeTypeOffset];if(edgeType!==edgeInternalType)
continue;var edgeName=strings[containmentEdges[edgeIndex+edgeNameOffset]];if(edgeName!=="elements")
continue;var elementsNodeIndex=containmentEdges[edgeIndex+edgeToNodeOffset];node.nodeIndex=elementsNodeIndex;if(node.retainersCount()===1)
size+=node.selfSize();break;}
return size;},getStatistics:function()
{return this._statistics;},__proto__:WebInspector.HeapSnapshot.prototype};WebInspector.JSHeapSnapshotNode=function(snapshot,nodeIndex)
{WebInspector.HeapSnapshotNode.call(this,snapshot,nodeIndex);}
WebInspector.JSHeapSnapshotNode.prototype={canBeQueried:function()
{var flags=this._snapshot._flagsOfNode(this);return!!(flags&this._snapshot._nodeFlags.canBeQueried);},rawName:WebInspector.HeapSnapshotNode.prototype.name,name:function()
{var snapshot=this._snapshot;if(this._type()===snapshot._nodeConsStringType){var string=snapshot._lazyStringCache[this.nodeIndex];if(typeof string==="undefined"){string=this._consStringName();snapshot._lazyStringCache[this.nodeIndex]=string;}
return string;}
return this.rawName();},_consStringName:function()
{var snapshot=this._snapshot;var consStringType=snapshot._nodeConsStringType;var edgeInternalType=snapshot._edgeInternalType;var edgeFieldsCount=snapshot._edgeFieldsCount;var edgeToNodeOffset=snapshot._edgeToNodeOffset;var edgeTypeOffset=snapshot._edgeTypeOffset;var edgeNameOffset=snapshot._edgeNameOffset;var strings=snapshot.strings;var edges=snapshot.containmentEdges;var firstEdgeIndexes=snapshot._firstEdgeIndexes;var nodeFieldCount=snapshot._nodeFieldCount;var nodeTypeOffset=snapshot._nodeTypeOffset;var nodeNameOffset=snapshot._nodeNameOffset;var nodes=snapshot.nodes;var nodesStack=[];nodesStack.push(this.nodeIndex);var name="";while(nodesStack.length&&name.length<1024){var nodeIndex=nodesStack.pop();if(nodes[nodeIndex+nodeTypeOffset]!==consStringType){name+=strings[nodes[nodeIndex+nodeNameOffset]];continue;}
var nodeOrdinal=nodeIndex/nodeFieldCount;var beginEdgeIndex=firstEdgeIndexes[nodeOrdinal];var endEdgeIndex=firstEdgeIndexes[nodeOrdinal+1];var firstNodeIndex=0;var secondNodeIndex=0;for(var edgeIndex=beginEdgeIndex;edgeIndex<endEdgeIndex&&(!firstNodeIndex||!secondNodeIndex);edgeIndex+=edgeFieldsCount){var edgeType=edges[edgeIndex+edgeTypeOffset];if(edgeType===edgeInternalType){var edgeName=strings[edges[edgeIndex+edgeNameOffset]];if(edgeName==="first")
firstNodeIndex=edges[edgeIndex+edgeToNodeOffset];else if(edgeName==="second")
secondNodeIndex=edges[edgeIndex+edgeToNodeOffset];}}
nodesStack.push(secondNodeIndex);nodesStack.push(firstNodeIndex);}
return name;},className:function()
{var type=this.type();switch(type){case"hidden":return"(system)";case"object":case"native":return this.name();case"code":return"(compiled code)";default:return"("+type+")";}},classIndex:function()
{var snapshot=this._snapshot;var nodes=snapshot.nodes;var type=nodes[this.nodeIndex+snapshot._nodeTypeOffset];;if(type===snapshot._nodeObjectType||type===snapshot._nodeNativeType)
return nodes[this.nodeIndex+snapshot._nodeNameOffset];return-1-type;},id:function()
{var snapshot=this._snapshot;return snapshot.nodes[this.nodeIndex+snapshot._nodeIdOffset];},isHidden:function()
{return this._type()===this._snapshot._nodeHiddenType;},isArray:function()
{return this._type()===this._snapshot._nodeArrayType;},isSynthetic:function()
{return this._type()===this._snapshot._nodeSyntheticType;},isUserRoot:function()
{return!this.isSynthetic();},isDocumentDOMTreesRoot:function()
{return this.isSynthetic()&&this.name()==="(Document DOM trees)";},serialize:function()
{var result=WebInspector.HeapSnapshotNode.prototype.serialize.call(this);var flags=this._snapshot._flagsOfNode(this);if(flags&this._snapshot._nodeFlags.canBeQueried)
result.canBeQueried=true;if(flags&this._snapshot._nodeFlags.detachedDOMTreeNode)
result.detachedDOMTreeNode=true;return result;},__proto__:WebInspector.HeapSnapshotNode.prototype};WebInspector.JSHeapSnapshotEdge=function(snapshot,edgeIndex)
{WebInspector.HeapSnapshotEdge.call(this,snapshot,edgeIndex);}
WebInspector.JSHeapSnapshotEdge.prototype={clone:function()
{var snapshot=(this._snapshot);return new WebInspector.JSHeapSnapshotEdge(snapshot,this.edgeIndex);},hasStringName:function()
{if(!this.isShortcut())
return this._hasStringName();return isNaN(parseInt(this._name(),10));},isElement:function()
{return this._type()===this._snapshot._edgeElementType;},isHidden:function()
{return this._type()===this._snapshot._edgeHiddenType;},isWeak:function()
{return this._type()===this._snapshot._edgeWeakType;},isInternal:function()
{return this._type()===this._snapshot._edgeInternalType;},isInvisible:function()
{return this._type()===this._snapshot._edgeInvisibleType;},isShortcut:function()
{return this._type()===this._snapshot._edgeShortcutType;},name:function()
{var name=this._name();if(!this.isShortcut())
return String(name);var numName=parseInt(name,10);return String(isNaN(numName)?name:numName);},toString:function()
{var name=this.name();switch(this.type()){case"context":return"->"+name;case"element":return"["+name+"]";case"weak":return"[["+name+"]]";case"property":return name.indexOf(" ")===-1?"."+name:"[\""+name+"\"]";case"shortcut":if(typeof name==="string")
return name.indexOf(" ")===-1?"."+name:"[\""+name+"\"]";else
return"["+name+"]";case"internal":case"hidden":case"invisible":return"{"+name+"}";};return"?"+name+"?";},_hasStringName:function()
{var type=this._type();var snapshot=this._snapshot;return type!==snapshot._edgeElementType&&type!==snapshot._edgeHiddenType;},_name:function()
{return this._hasStringName()?this._snapshot.strings[this._nameOrIndex()]:this._nameOrIndex();},_nameOrIndex:function()
{return this._edges[this.edgeIndex+this._snapshot._edgeNameOffset];},_type:function()
{return this._edges[this.edgeIndex+this._snapshot._edgeTypeOffset];},__proto__:WebInspector.HeapSnapshotEdge.prototype};WebInspector.JSHeapSnapshotRetainerEdge=function(snapshot,retainerIndex)
{WebInspector.HeapSnapshotRetainerEdge.call(this,snapshot,retainerIndex);}
WebInspector.JSHeapSnapshotRetainerEdge.prototype={clone:function()
{var snapshot=(this._snapshot);return new WebInspector.JSHeapSnapshotRetainerEdge(snapshot,this.retainerIndex());},isHidden:function()
{return this._edge().isHidden();},isInternal:function()
{return this._edge().isInternal();},isInvisible:function()
{return this._edge().isInvisible();},isShortcut:function()
{return this._edge().isShortcut();},isWeak:function()
{return this._edge().isWeak();},__proto__:WebInspector.HeapSnapshotRetainerEdge.prototype};function postMessageWrapper(message)
{postMessage(message);}
var dispatcher=new WebInspector.HeapSnapshotWorkerDispatcher(this,postMessageWrapper);function installMessageEventListener(listener)
{self.addEventListener("message",listener,false);}
installMessageEventListener(dispatcher.dispatchMessage.bind(dispatcher));;