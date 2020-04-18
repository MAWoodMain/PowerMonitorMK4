import * as React from "react";
import 'react-bulma-components/dist/react-bulma-components.min.css';
import axios from "axios";

export class DelItemForm extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            idToDelete: "",
        };
    }

    deleteFromDB = idTodelete => {
        let objIdToDelete = null;
        this.props.data.forEach(dat => {
            if (dat.id === idTodelete) {
                objIdToDelete = dat._id;
            }
        });

        axios.delete("http://powermonitor-mk4:3001/api/data/deleteData", {
            data: {
                id: objIdToDelete
            }
        });
    };

    render() {
        return (
            <div className="Box">
                <input
                    className="input"
                    type="text"
                    onChange={e => this.setState({ idToDelete: e.target.value })}
                    placeholder="delete something from the database"
                    style={{ width: "300px" }}
                />
                <button className="button" onClick={() => this.deleteFromDB(this.state.idToDelete)}>
                    DELETE
                </button>
            </div>
        );
    }
}