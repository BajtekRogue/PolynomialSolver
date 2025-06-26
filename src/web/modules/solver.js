import { getModule, getParsedPolynomials } from '../main.js';
import { validateInput} from './utils.js';
import { createStringVector } from './utils.js';

export function solveWithCpp(field, operation, polynomials, prime) {
    const Module = getModule();
    let stringVector = null;
    let result = null;
    let system = null;
    let systemResult = null;

    try {
        // Create StringVector for the polynomials
        stringVector = createStringVector(polynomials);

        if (field === 'Q') {
            systemResult = Module.buildSystemFromStrings__Rational(stringVector);

            if (!systemResult.success) {
                const errorMsg = systemResult.error || 'Unknown system building error';
                return {
                    success: false,
                    error: errorMsg
                };
            }
            
            system = systemResult.polynomials;
            
            if (!system || system.size() === 0) {
                return {
                    success: false,
                    error: 'Empty system is not allowed'
                };
            }
            
            if (operation === 'solve') {
                result = Module.printSystemSolution__Rational(system);
            } else if (operation === 'characteristic') {
                result = Module.printCharacteristicEquations__Rational(system);
            } else {
                return {
                    success: false,
                    error: 'Unknown operation: ' + operation
                };
            }
            
        } else if (field === 'Fp') {
            const primeNum = parseInt(prime);
            if (isNaN(primeNum) || primeNum < 2) {
                return {
                    success: false,
                    error: 'Invalid prime number: ' + prime
                };
            }
            
            // Set the prime
            const primeSet = Module.GaloisField.setPrime(primeNum);
            if (!primeSet) {
                return {
                    success: false,
                    error: 'Failed to set prime number: ' + primeNum
                };
            }
            
            systemResult = Module.buildSystemFromStrings__GaloisField(stringVector);
            
            if (!systemResult.success) {
                const errorMsg = systemResult.error || 'Unknown system building error';
                return {
                    success: false,
                    error: errorMsg
                };
            }
            
            system = systemResult.polynomials;
            
            if (!system || system.size() === 0) {
                return {
                    success: false,
                    error: 'Empty system is not allowed'
                };
            }
            
            // Execute the operation
            if (operation === 'solve') {
                result = Module.printSystemSolution__GaloisField(system);
            } else if (operation === 'characteristic') {
                result = Module.printCharacteristicEquations__GaloisField(system);
            } else {
                return {
                    success: false,
                    error: 'Unknown operation: ' + operation
                };
            }
        }
        else if (field == 'R') {
            systemResult = Module.buildSystemFromStrings__Real(stringVector);

            if (!systemResult.success) {
                const errorMsg = systemResult.error || 'Unknown system building error';
                return {
                    success: false,
                    error: errorMsg
                };
            }
            
            system = systemResult.polynomials;
            
            if (!system || system.size() === 0) {
                return {
                    success: false,
                    error: 'Empty system is not allowed'
                };
            }
            
            if (operation === 'solve') {
                result = Module.printSystemSolution__Real(system);
            } else if (operation === 'characteristic') {
                result = Module.printCharacteristicEquations__Real(system);
            } else {
                return {
                    success: false,
                    error: 'Unknown operation: ' + operation
                };
            }            
        }
        
        else {
            return {
                success: false,
                error: 'Unknown field type: ' + field
            };
        }
        
        // Extract variables from systemResult
        let variables = [];
        if (systemResult && systemResult.variables) {
            try {
                const variableVector = systemResult.variables;
                if (variableVector && typeof variableVector.size === 'function') {
                    const size = variableVector.size();
                    for (let i = 0; i < size; i++) {
                        variables.push(variableVector.get(i));
                    }
                }
                // Sort variables for consistent display
                variables.sort();
            } catch (error) {
                console.warn('Failed to extract variables:', error);
                variables = [];
            }
        }
        
        return {
            success: true,
            result: result || 'No result returned',
            systemSize: system ? system.size() : 0,
            variableCount: variables.length,
            variables: variables,
            field: field,
            prime: field === 'Fp' ? parseInt(prime) : null
        };
        
    } catch (error) {
        const errorMsg = error.message || error.toString() || 'Unknown WebAssembly error';
        console.error('WebAssembly exception:', error);
        return {
            success: false,
            error: `WebAssembly error: ${errorMsg}`
        };
    } finally {
        if (stringVector && typeof stringVector.delete === 'function') {
            try {
                stringVector.delete();
            } catch (deleteError) {
                console.warn('Failed to delete StringVector:', deleteError);
            }
        }
    }
}

export async function solve() {
    const field = document.querySelector('input[name="field"]:checked').value;
    const operation = document.querySelector('input[name="operation"]:checked').value;
    const prime = document.getElementById('prime').value;
    const polynomials = getParsedPolynomials();
    
    const resultsDiv = document.getElementById('results');
    const resultsContent = document.getElementById('results-content');
    const solveBtn = document.querySelector('.solve-btn');
    
    const validation = validateInput(field, operation, polynomials, prime);
    if (!validation.valid) {
        resultsContent.innerHTML = `<div class="error">${validation.message}</div>`;
        resultsDiv.style.display = 'block';
        return;
    }
    
    solveBtn.disabled = true;
    solveBtn.textContent = '🔄 Computing...';
    resultsContent.innerHTML = '<div class="loading">Computing polynomial system using WebAssembly...</div>';
    resultsDiv.style.display = 'block';
    
    try {
        const calculationResult = solveWithCpp(field, operation, polynomials, prime);
        
        if (calculationResult.success) {
            displayResults(calculationResult, operation, polynomials);
        } else {
            resultsContent.innerHTML = `<div class="error">${calculationResult.error}</div>`;
        }
        
    } catch (error) {
        console.error('Computation failed:', error);
        resultsContent.innerHTML = `<div class="error">Computation failed: ${error.message}</div>`;
    } finally {
        solveBtn.disabled = false;
        solveBtn.textContent = '🚀 Compute';
        resultsDiv.scrollIntoView({ behavior: 'smooth' });
    }
}

function getFieldLatex(field, prime) {
    if (field === 'Q') {
        return '\\(\\mathbb{Q}\\)';
    } else if (field === 'Fp') {
        return `\\(\\mathbb{F}_{${prime}}\\)`;
    } else if (field === 'R') {
        return '\\(\\mathbb{R}\\)';
    }
    return field; 
}

function displayResults(calculationResult, operation, polynomials) {
    const resultsContent = document.getElementById('results-content');
    const Module = getModule();
    
    let result = '<div class="result-section">';
    result += '<h4>🔧 Computation Parameters:</h4>';

    const fieldLatex = getFieldLatex(calculationResult.field, calculationResult.prime);
    result += `<p><strong>Field:</strong> ${fieldLatex}</p>`;
    result += `<p><strong>Operation:</strong> ${operation === 'solve' ? 'Solve System' : 'Characteristic Equations'}</p>`;
    
    const polynomialText = calculationResult.systemSize === 1 ? 'polynomial' : 'polynomials';
    const variableText = calculationResult.variableCount === 1 ? 'variable' : 'variables';
    
    result += `<p><strong>System size:</strong> ${calculationResult.systemSize} ${polynomialText}`;
    if (calculationResult.variableCount !== undefined) {
        result += `, ${calculationResult.variableCount} ${variableText}`;
        if (calculationResult.variables && calculationResult.variables.length > 0) {
            result += ` ${calculationResult.variables.join(', ')}`;
        }
    }
    result += '</p>';
    result += '</div>';
    result += '<div class="result-section">';
    result += '<h4>📋 Input Polynomials:</h4>';
    
    polynomials.forEach((poly, index) => {
        let displayPoly = poly;
        try {
            let betterString;
            if (calculationResult.field === 'Q') {
                betterString = Module.inputStringToBetterString__Rational(poly);
            } else if (calculationResult.field === 'Fp') {
                betterString = Module.inputStringToBetterString__GaloisField(poly);
            } else if (calculationResult.field === 'R') {
                betterString = Module.inputStringToBetterString__Real(poly);
            }
            
            if (betterString && betterString !== "Error parsing polynomial") {
                displayPoly = betterString;
            }
        } catch (error) {
            console.warn('Failed to format polynomial:', error);
        }
        
        const mathDisplayPoly = `\\(g_{${index + 1}}(${calculationResult.variables.join(', ')})=${displayPoly}\\)`;
        result += `<div class="polynomial-item">
            <em class="math-expression">${mathDisplayPoly}</em>
        </div>`;
    });
    result += '</div>';
    
    result += '<div class="result-section">';
    result += '<h4>🎯 Computation Results:</h4>';
    result += '<div class="result-output">';
    result += `<pre>${calculationResult.result || 'No result returned'}</pre>`;
    result += '</div>';
    result += '</div>';

    result += '<div class="result-section">';
    result += '<button class="copy-btn" onclick="copyResults()">📋 Copy Results</button>';
    result += '</div>';
    
    resultsContent.innerHTML = result;
    
    if (window.MathJax && window.MathJax.typesetPromise) {
        window.MathJax.typesetPromise([resultsContent]).then(() => {
            console.log('MathJax rendering completed');
        }).catch((err) => {
            console.warn('MathJax rendering failed:', err);
        });
    } else {
        console.warn('MathJax typesetPromise not available');
    }
}