import './styles/Console.css';
import { useGeneratedCode } from '../GeneratedCodeContext';

function TextCode() {

    const { generatedCode } = useGeneratedCode();

    return (
        <div className="console">
            <div className="title">Generated Code</div>
            <div className="code" dangerouslySetInnerHTML={{ __html: generatedCode }} />
        </div>
    );
}

export default TextCode;