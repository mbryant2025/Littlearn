import React, { createContext, useState, ReactNode } from 'react';

interface GeneratedCodeContextProps {
    generatedCode: string;
    setGeneratedCode: (code: string) => void;
}

const GeneratedCodeContext = createContext<GeneratedCodeContextProps | undefined>(undefined);

interface GeneratedCodeProviderProps {
    children: ReactNode;
}

export const GeneratedCodeProvider: React.FC<GeneratedCodeProviderProps> = ({ children }) => {
    const [generatedCode, setGeneratedCode] = useState<string>('');

    return (
        <GeneratedCodeContext.Provider value={{ generatedCode, setGeneratedCode }}>
            {children}
        </GeneratedCodeContext.Provider>
    );
}

export const useGeneratedCode = () => {
    const context = React.useContext(GeneratedCodeContext);
    if (context === undefined) {
        throw new Error('useGeneratedCode must be used within a GeneratedCodeProvider');
    }
    return context;
}