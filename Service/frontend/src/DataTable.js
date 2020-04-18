import * as React from "react";
import 'react-bulma-components/dist/react-bulma-components.min.css';

export class DataTable extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            data: [],
        };
    }

    render() {
        return (
            <div className="Box">
                <table className="table">
                    <thead>
                    <tr>
                        <th><abbr title="Identifier">ID</abbr></th>
                        <th><abbr title="Message">msg</abbr></th>
                    </tr>
                    </thead>
                    <tbody>
                    {this.props.data.map(dat => (
                        <tr key={dat}>
                            <th>{dat.id}</th>
                            <td>{dat.message}</td>
                        </tr>
                    ))}
                    </tbody>
                </table>
            </div>
        );
    }
}