import './styles/Console.css';
import { useGeneratedCode } from '../GeneratedCodeContext';

function TextCode() {

    const { generatedCode } = useGeneratedCode();

    return (
        <div className="console">
            <div className="title">Generated Code</div>

            <div>
                {generatedCode.split('<br>').map((line, i) => {
                    return <div key={i}>{line}</div>
                })}
            </div>
            
        </div>
    );
}

export default TextCode;