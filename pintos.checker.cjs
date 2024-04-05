// @ts-check

/** Condition that must be met for the test to pass, initially if it has a grade greater than or equal to 20 the test passes
 * @param {number} grade is the current grade
 * @param {number} total is the maximum grade
 * @returns {boolean} returns true if the test should pass */
function threads(grade, total) {
  return grade >= 20
}

/** Condition that must be met for the test to pass, initially if it has a grade greater than or equal to 0 the test passes
 * @param {number} grade is the current grade
 * @param {number} total is the maximum grade
 * @returns {boolean} returns true if the test should pass */
function userprog(grade, total) {
  return grade >= 0
}

/** Condition that must be met for the test to pass, initially if it has a grade greater than or equal to 0 the test passes
 * @param {number} grade is the current grade
 * @param {number} total is the maximum grade
 * @returns {boolean} returns true if the test should pass */
function vm(grade, total) {
  return grade >= 0
}

/** Condition that must be met for the test to pass, initially if it has a grade greater than or equal to 0 the test passes
 * @param {number} grade is the current grade
 * @param {number} total is the maximum grade
 * @returns {boolean} returns true if the test should pass */
function filesys(grade, total) {
  return grade >= 0
}

/** Prints the message to the screen
 * @param {Function} print function that prints to the screen
 * @param {string} results is the complete text
 * @param {number} grade is the grade obtained
 * @param {number} total is the maximum grade */
function showResults(print, results, grade, total) {
  print(`\x1b[33mEvaluation: ${grade}/${total}\x1b[0m\n`)
  print(results)
}

/** This line is used to extract the grade and total
  Example:
  ...
  tests/filesys/base/Rubric                       30/ 30   30.0%/ 30.0%
  --------------------------------------------- --- --- ------ ------
      Total                                                   90.5%/100.0%
  
  extracts:
    grade: 90.5
    total: 100.0
  */
/** @type {RegExp} */
const gradeExpr = /Total(\t|\s)+(?<grade>\d+[.]\d)+%\/(?<total>\d+[.]\d+)%/mg

module.exports = { threads, userprog, vm, filesys, showResults, gradeExpr }
