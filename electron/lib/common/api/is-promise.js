'use strict'

function isPromise (val) {
  return (
    val &&
    val.then &&
    val.then instanceof Function &&
    val.constructor &&
    val.constructor.reject &&
    val.constructor.reject instanceof Function &&
    val.constructor.resolve &&
    val.constructor.resolve instanceof Function
  )
}
module.exports = isPromise;
isPromise.isPromise = isPromise;