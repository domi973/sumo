/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEChange_Lane.cpp
/// @author  Jakob Erdmann
/// @date    April 2011
///
// A network change in which a single lane is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/net/GNEEdge.h>
#include <netedit/elements/net/GNELane.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>

#include "GNEChange_Lane.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Lane, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Lane::GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward, bool recomputeConnections):
    GNEChange(edge->getNet(), forward),
    myEdge(edge),
    myLane(lane),
    myLaneAttrs(laneAttrs),
    myRecomputeConnections(recomputeConnections) {
    assert(myNet);
    myEdge->incRef("GNEChange_Lane");
    if (myLane) {
        // non-zero pointer is passsed in case of removal or duplication
        myLane->incRef("GNEChange_Lane");
        // Save hierarchy elements of lane
        myParentShapes = myLane->getParentShapes();
        myParentAdditionals = myLane->getParentAdditionals();
        myParentDemandElements = myLane->getParentDemandElements();
        myChildShapes = myLane->getChildShapes();
        myChildAdditionals = myLane->getChildAdditionals();
        myChildDemandElements = myLane->getChildDemandElements();
    } else {
        assert(forward);
    }
}


GNEChange_Lane::~GNEChange_Lane() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Lane");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myEdge->getTagStr() + " '" + myEdge->getID() + "' in GNEChange_Lane");
        delete myEdge;
    }
    if (myLane) {
        myLane->decRef("GNEChange_Lane");
        if (myLane->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + myLane->getTagStr() + " '" + myLane->getID() + "' in GNEChange_Lane");
            delete myLane;
        }
    }
}


void
GNEChange_Lane::undo() {
    if (myForward) {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Removing " + myLane->getTagStr() + " '" + myLane->getID() + "' from " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Removing nullptr " + toString(SUMO_TAG_LANE) + " from " + toString(SUMO_TAG_EDGE));
        }
        // remove lane from edge
        myEdge->removeLane(myLane, false);
        // Remove lane from parent elements
        for (const auto& i : myParentShapes) {
            i->removeChildLane(myLane);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildLane(myLane);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildLane(myLane);
        }
        // Remove lane from child elements
        for (const auto& i : myChildShapes) {
            i->removeParentLane(myLane);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentLane(myLane);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentLane(myLane);
        }
    } else {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Adding " + myLane->getTagStr() + " '" + myLane->getID() + "' into " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Adding nullptr " + toString(SUMO_TAG_LANE) + " into " + toString(SUMO_TAG_EDGE));
        }
        // add lane and their attributes to edge
        // (lane removal is reverted, no need to recompute connections)
        myEdge->addLane(myLane, myLaneAttrs, false);
        // add lane in parent elements
        for (const auto& i : myParentShapes) {
            i->addChildLane(myLane);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildLane(myLane);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildLane(myLane);
        }
        // add lane in child elements
        for (const auto& i : myChildShapes) {
            i->addParentLane(myLane);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentLane(myLane);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentLane(myLane);
        }
    }
    // enable save netElements
    myNet->requireSaveNet(true);
}


void
GNEChange_Lane::redo() {
    if (myForward) {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Adding " + myLane->getTagStr() + " '" + myLane->getID() + "' into " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Adding nullptr " + toString(SUMO_TAG_LANE) + " into " + toString(SUMO_TAG_EDGE));
        }
        // add lane and their attributes to edge
        myEdge->addLane(myLane, myLaneAttrs, myRecomputeConnections);
        // add lane in parent elements
        for (const auto& i : myParentShapes) {
            i->addChildLane(myLane);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildLane(myLane);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildLane(myLane);
        }
        // add additional in child elements
        for (const auto& i : myChildShapes) {
            i->addParentLane(myLane);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentLane(myLane);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentLane(myLane);
        }
    } else {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Removing " + myLane->getTagStr() + " '" + myLane->getID() + "' from " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Removing nullptr " + toString(SUMO_TAG_LANE) + " from " + toString(SUMO_TAG_EDGE));
        }
        // Remove lane from parent elements
        for (const auto& i : myParentShapes) {
            i->removeChildLane(myLane);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildLane(myLane);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildLane(myLane);
        }
        // Remove additional from child elements
        for (const auto& i : myChildShapes) {
            i->removeParentLane(myLane);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentLane(myLane);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentLane(myLane);
        }
        // remove lane from edge
        myEdge->removeLane(myLane, myRecomputeConnections);
    }
    // enable save netElements
    myNet->requireSaveNet(true);
}


FXString
GNEChange_Lane::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}


FXString
GNEChange_Lane::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}
