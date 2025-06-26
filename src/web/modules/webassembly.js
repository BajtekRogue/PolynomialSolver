import { setModule, getParsedPolynomials } from '../main.js';
import { displayPolynomials } from './polynomialParser.js';

export async function loadWebAssemblyModule() {
    try {
        const moduleFactory = await import('/build/main.js');
        
        let factory;
        if (typeof moduleFactory.SolverModule === 'function') {
            factory = moduleFactory.SolverModule;
        } else if (typeof moduleFactory.default === 'function') {
            factory = moduleFactory.default;
        } else {
            throw new Error('No valid module factory found');
        }
        
        let Module = await factory();
        
        if (typeof Module.then === 'function') {
            Module = await Module;
        }
        
        await new Promise((resolve) => {
            if (Module.onRuntimeInitialized) {
                Module.onRuntimeInitialized = resolve;
            } else {
                setTimeout(resolve, 100);
            }
        });
        
        setModule(Module);
        console.log('WebAssembly module loaded successfully');
        
        // Refresh polynomial display now that module is loaded
        const parsedPolynomials = getParsedPolynomials();
        if (parsedPolynomials.length > 0) {
            displayPolynomials(parsedPolynomials);
        }
        
    } catch (error) {
        console.error('Dynamic import failed, trying script tag method:', error);
        
        try {
            await loadViaScriptTag();
        } catch (fallbackError) {
            console.error('Failed to load WebAssembly module:', fallbackError);
        }
    }
}

function loadViaScriptTag() {
    return new Promise((resolve, reject) => {
        const existingScript = document.querySelector('script[src*="main.js"]');
        if (existingScript) {
            existingScript.remove();
        }
        
        const script = document.createElement('script');
        script.src = '/build/main.js';
        script.onload = async () => {
            try {
                await new Promise(resolve => setTimeout(resolve, 100));
                
                if (typeof SolverModule === 'function') {
                    const Module = await SolverModule();
                    
                    if (Module.onRuntimeInitialized) {
                        await new Promise((resolve) => {
                            Module.onRuntimeInitialized = resolve;
                        });
                    }
                    
                    setModule(Module);
                    console.log('WebAssembly module loaded via script tag');
                    
                    // Refresh polynomial display now that module is loaded
                    const parsedPolynomials = getParsedPolynomials();
                    if (parsedPolynomials.length > 0) {
                        displayPolynomials(parsedPolynomials);
                    }
                    
                    resolve();
                } else {
                    reject(new Error('SolverModule not available after script load'));
                }
            } catch (error) {
                reject(error);
            }
        };
        script.onerror = () => reject(new Error('Failed to load script'));
        document.head.appendChild(script);
    });
}


