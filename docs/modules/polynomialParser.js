import { getModule, setParsedPolynomials } from './main.js';
import { isPrime } from './fieldSelection.js';
import { createStringVector } from './utils.js';

export function parsePolynomials() {
    const input = document.getElementById('polynomial-input');
    const listDiv = document.getElementById('polynomial-list');
    
    const text = input.value.trim();
    
    if (!text) {
        listDiv.innerHTML = '<em>No polynomials entered yet...</em>';
        setParsedPolynomials([]);
        return;
    }

    // Split only by lines - each line is a separate polynomial
    const lines = text.split('\n');
    let polynomials = [];
    
    for (let line of lines) {
        const trimmed = line.trim();
        if (trimmed) {
            polynomials.push(trimmed);
        }
    }

    setParsedPolynomials(polynomials);
    displayPolynomials(polynomials);
}

export function displayPolynomials(polynomials) {
    const listDiv = document.getElementById('polynomial-list');
    
    if (polynomials.length === 0) {
        listDiv.innerHTML = '<em>No polynomials entered yet...</em>';
        return;
    }

    const Module = getModule();
    let html = '';
    
    // First pass: collect all variables from all valid polynomials
    let allVariables = new Set();
    const fieldRadio = document.querySelector('input[name="field"]:checked');
    const field = fieldRadio ? fieldRadio.value : 'Q';
    
    // Set prime for GaloisField if needed
    if (field === 'Fp') {
        const primeValue = document.getElementById('prime').value;
        const primeNum = parseInt(primeValue);
        if (primeNum >= 2 && isPrime(primeNum)) {
            try {
                Module.GaloisField.setPrime(primeNum);
            } catch (primeError) {
                console.warn('Failed to set prime:', primeError);
            }
        }
    }
    
    // Collect variables from all polynomials
    polynomials.forEach(poly => {
        try {
            let stringVector = createStringVector([poly]);
            let systemResult;
            
            if (field === 'Q') {
                systemResult = Module.buildSystemFromStrings__Rational(stringVector);
            } else if (field === 'Fp') {
                systemResult = Module.buildSystemFromStrings__GaloisField(stringVector);
            } else if (field === 'R') {
                systemResult = Module.buildSystemFromStrings__Real(stringVector);
            }
            
            // Extract variables 
            if (systemResult && systemResult.success && systemResult.variables) {
                if (systemResult.variables.size && systemResult.variables.size() > 0) {
                    for (let i = 0; i < systemResult.variables.size(); i++) {
                        allVariables.add(systemResult.variables.get(i));
                    }
                } else if (Array.isArray(systemResult.variables)) {
                    systemResult.variables.forEach(v => allVariables.add(v));
                }
            }
            
            if (stringVector && typeof stringVector.delete === 'function') {
                stringVector.delete();
            }
        } catch (error) {
            console.warn('Error extracting variables from polynomial:', poly, error);
        }
    });
    
    // Convert Set to sorted array for consistent display
    const variables = Array.from(allVariables).sort();
    
    // If no variables found, use defaults
    if (variables.length === 0) {
        variables.push('x', 'y', 'z');
    }
    
    // Second pass: display polynomials with the combined variable list
    polynomials.forEach((poly, index) => {
        let className = 'polynomial-item';
        let displayPoly = poly;
        let status = '';
        
        try {
            // Try the simple formatter
            let betterString;
            if (field === 'Q') {
                betterString = Module.inputStringToBetterString__Rational(poly);
            } else if (field === 'Fp') {
                betterString = Module.inputStringToBetterString__GaloisField(poly);
            } else if (field === 'R') {
                betterString = Module.inputStringToBetterString__Real(poly);
            }
            
            if (betterString && betterString !== "Error parsing polynomial") {
                displayPoly = betterString;
                className += ' valid';
                status = ' ✅';
            } else {
                // If simple formatter failed, try to get detailed error
                try {
                    let stringVector = createStringVector([poly]);
                    let systemResult;
                    
                    if (field === 'Q') {
                        systemResult = Module.buildSystemFromStrings__Rational(stringVector);
                    } else if (field === 'Fp') {
                        systemResult = Module.buildSystemFromStrings__GaloisField(stringVector);
                    } else if (field === 'R') {
                        systemResult = Module.buildSystemFromStrings__Real(stringVector);
                    }
                    
                    // Clean up the vector
                    if (stringVector && typeof stringVector.delete === 'function') {
                        stringVector.delete();
                    }
                    
                    if (systemResult && !systemResult.success && systemResult.error) {
                        status = ` ❌ ${systemResult.error}`;
                    } else {
                        status = ' ❌ Error parsing polynomial';
                    }
                } catch (detailedError) {
                    const errorMsg = detailedError.message || detailedError.toString() || 'Unknown parsing error';
                    status = ` ❌ ${errorMsg}`;
                }
                className += ' error';
            }
            
        } catch (error) {
            const errorMsg = error.message || error.toString() || 'Unknown error';
            status = ` ❌ ${errorMsg}`;
            className += ' error';
        }
        
        const mathDisplayPoly = `\\(\\wp_{${index + 1}}(${variables.join(', ')})=${displayPoly}\\)`;
        html += `<div class="${className}">
            <em class="math-expression">${mathDisplayPoly}</em>${status}
        </div>`;
    });
    
    listDiv.innerHTML = html;
    
    // Re-render MathJax after updating the content
    if (window.MathJax && window.MathJax.typesetPromise) {
        window.MathJax.typesetPromise([listDiv]).catch((err) => {
            console.warn('MathJax rendering failed:', err);
        });
    }
}