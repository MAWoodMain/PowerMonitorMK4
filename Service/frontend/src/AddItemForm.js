import * as React from "react";
import 'react-bulma-components/dist/react-bulma-components.min.css';
import axios from "axios";

export class AddItemForm extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            message: "",
        };
    }

    putDataToDB = message => {
        let currentIds = this.props.data.map(data => data.id);
        let idToBeAdded = 0;
        while (currentIds.includes(idToBeAdded)) {
            ++idToBeAdded;
        }

        axios.post("http://powermonitor-mk4:3001/api/data/putData", {
            id: idToBeAdded,
            message: message
        });
    };

    render() {
        return (
            <div className="Box">
                <input
                    className="input"
                    type="text"
                    onChange={e => this.setState({ message: e.target.value })}
                    placeholder="add something in the database"
                    style={{ width: "300px" }}
                />
                <button className="button" onClick={() => this.putDataToDB(this.state.message)}>
                    ADD
                </button>
            </div>
        );
    }
}