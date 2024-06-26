
var kRequestCodeSeparator = "__INFO__";

//     p,q is two big prime numbers
//     N = pq
//     r is the Euler's function output , r = (p-1)*(q-1)
//     e is the public key
//     d is the private key
//     print("onNativeCreateRequestCode, p:" + p);
//     print("onNativeCreateRequestCode, q:" + q);
//     print("onNativeCreateRequestCode, N:" + N);
//     print("onNativeCreateRequestCode, publicKey:" + kRequestCode_publicKey);
//     print("onNativeCreateRequestCode, d:" + d);

var kRequestCode_p = 19;
var kRequestCode_q = 97;
var kRequestCode_N = 1843;
var kRequestCode_publicKey = 5;
var kRequestCode_privateKey = 1037;
var kRequestCode_r = 1728;

var kLicense_p = 29;
var kLicense_q = 37;
var kLicense_N = 1073;
var kLicense_publicKey = 5;
var kLicense_privateKey = 605;
var kLicense_r = 1008;

function genGcd(a, b) {
    var t;

    while (b) {
        t = a;
        a = b;
        b = t % b;
    }
    return a;
};

function modInverse(a, r) {
    var b = 1;
    while (((a * b) % r) != 1) {
        b++;
        if (b < 0) {
            print("error ,function can't find b ,and now b is negative number");
            return -1;
        }
    }
    return b;
};

function primeW(a, b) {
    if (genGcd(a, b) == 1)
        return true;
    else
        return false;
};

function genSecretKey() {
    var result = { "p": 0, "q": 0 };
    do {
        result.p = rand() % 50 + 1;
    } while (!nativeIsPrime(result.p));

    do {
        result.q = rand() % 50 + 1;
    } while (result.p == result.q || !nativeIsPrime(result.q));
    return result;
};

function getRandE(r) {
    var e = 2;
    while (e < 1 || e > r || !primeW(e, r)) {
        e++;
        if (e < 0) {
            //print("error, function can't find e ,and now e is negative number");
            return -1;
        }
    }
    return e;
};

function genRsa(a, b, c) {
    var i = a;
    var r = 1;

    b = b + 1;

    while (b != 1) {
        r = r * i;
        r = r % c;
        b--;
    }
    return r;
};

function charCodeAt (c) {
    var ret = c - 'a' + 97;
    return ret;
};

function getMachineInfo() {
    var result = "";
    result += nativeGetVolumeID();
    result += "And";
    result += nativeGetMacID();
    result += "And";
    result += nativeGetDiskID();
    result += "And";
    result += nativeGetCpuID();

    //result = "test";
    return result;
}

function onNativeCreateRequestCode (regInfo) {
    print("onNativeCreateRequestCode begin");

    var plaintextStr = regInfo;
    plaintextStr += kRequestCodeSeparator;
    plaintextStr += getMachineInfo();

    var cipheBuff = [];
    cipheBuff.resize(plaintextStr.len());

    for (var i = 0; i < plaintextStr.len(); i++) {
        var c = charCodeAt(plaintextStr[i]);
        //print("onNativeCreateRequestCode, i:" + i + ", " + c);
        cipheBuff[i] = nativeGenRsa(c, kRequestCode_publicKey, kRequestCode_N);
        //print("onNativeCreateRequestCode, cipheBuff:" + cipheBuff[i]);
    }

    return cipheBuff;
}

function restoreToRequestCodeAndAlert (cipheText) {
    var cipheTextBlob = nativeBase64ToBlob(cipheText);
    if (!cipheTextBlob) {
        print("restoreToRequestCodeAndAlert fail: 1!");
        return false;
    }

    var cipheTextInt = [];
    var len = cipheTextBlob.len();
    if (len % 4 != 0) {
        print("restoreToRequestCodeAndAlert fail: 2!");
        return false;
    }

    for (var i = 0; i < len; i += 4) {
        var c = nativeUnsignToInt(cipheTextBlob[i], cipheTextBlob[i + 1], cipheTextBlob[i + 2], cipheTextBlob[i + 3]);
        cipheTextInt.push(c);
    }

    var encryptStr = "";

    for (var i = 0; i < cipheTextInt.len() ; i++) {
        var c = nativeGenRsa((cipheTextInt[i]), kRequestCode_privateKey, kRequestCode_N);
        //print("restoreToRequestCodeAndAlert, cipheTextBlob[i]:" + cipheTextInt[i]);
        encryptStr += (c).tochar();
    }

    //print("restoreToRequestCodeAndAlert, encryptStr:" + encryptStr);
    var requestCode = encryptStr;
    var pos = requestCode.find(kRequestCodeSeparator);
    if (!pos)
        return false;
    pos += kRequestCodeSeparator.len();
    requestCode = requestCode.slice(pos);
    if (!requestCode)
        return false;

    var b = (requestCode == getMachineInfo());
    //print("restoreToRequestCodeAndAlert, encryptStr 2:" + b + ", " + requestCode);
    if (b == false) {
    	print("restoreToRequestCodeAndAlert fail: 3!");
        //nativeAlert();
    }
    return b;
}

function onNativeVerifyLicense(license) {
    //print("onNativeVerifyLicense begin");
    var licenseBlob = nativeBase64ToBlob(license);
    var len = licenseBlob.len();
    if ((len % 4) != 0) {
        print("onNativeVerifyLicense fail");
        return false;
    }

    var licenseInt = [];
    for (var i = 0; i < len; i += 4) {
        var c = nativeUnsignToInt(licenseBlob[i], licenseBlob[i + 1], licenseBlob[i + 2], licenseBlob[i + 3]);
        licenseInt.push(c);
    }

    var cipheText = "";
    len = licenseInt.len();
    for (var i = 0; i < len; ++i) {
        //print("onNativeverifyLicense 1:" + licenseInt[i]);
        var c = nativeGenRsa(licenseInt[i], kLicense_privateKey, kLicense_N);
        //print("onNativeVerifyLicense 2:" + c);
        cipheText += (c.tochar());
    }

    return restoreToRequestCodeAndAlert(cipheText);
}

function onNativeTimerFire() {
    if (nativeLoadLicenseAndVerify()) {// to call onNativeVerifyLicense
        print("license check ok");
        return;
    }
    nativeAlert();
}