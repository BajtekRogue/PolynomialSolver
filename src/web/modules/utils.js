import { getModule } from '../main.js';
import { isPrime } from './fieldSelection.js';

export function createStringVector(strings) {
    const Module = getModule();
    
    let vec = null;
    try {
        vec = new Module.StringVector();
        for (const str of strings) {
            if (typeof str !== 'string') {
                throw new Error('All elements must be strings');
            }
            vec.push_back(str);
        }
        return vec;
    } catch (error) {
        if (vec && typeof vec.delete === 'function') {
            try {
                vec.delete();
            } catch (deleteError) {
                console.warn('Failed to cleanup failed StringVector:', deleteError);
            }
        }
        throw error;
    }
}

export function validateInput(field, operation, polynomials, prime) {
    const Module = getModule();
    
    
    if (polynomials.length === 0) {
        return { valid: false, message: '❌ Please add at least one polynomial' };
    }
    
    if (field === 'Fp') {
        const primeNum = parseInt(prime);
        if (isNaN(primeNum) || primeNum < 2) {
            return { valid: false, message: '❌ Please enter a valid prime number ≥ 2 for finite field' };
        }
        
        if (!isPrime(primeNum)) {
            return { valid: false, message: `❌ ${primeNum} is not a prime number` };
        }
        
        try {
            const validPrime = Module.GaloisField.setPrime(primeNum);
            if (!validPrime) {
                return { valid: false, message: '❌ Failed to set prime number in WebAssembly module' };
            }
        } catch (error) {
            return { valid: false, message: '❌ Error setting prime number: ' + error.message };
        }
    }
    
    return { valid: true };
}


export function copyResults() {
    const resultOutput = document.querySelector('.result-output pre');
    if (resultOutput) {
        navigator.clipboard.writeText(resultOutput.textContent).then(() => {
            const copyBtn = document.querySelector('.copy-btn');
            const originalText = copyBtn.textContent;
            copyBtn.textContent = '✅ Copied!';
            setTimeout(() => {
                copyBtn.textContent = originalText;
            }, 2000);
        }).catch(err => {
            console.error('Failed to copy: ', err);
        });
    }
}
