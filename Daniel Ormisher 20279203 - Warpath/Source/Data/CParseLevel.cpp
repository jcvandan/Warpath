///////////////////////////////////////////////////////////
//  CParseLevel.cpp
//  A class to parse and setup a level (entity templates
//  and instances) from an XML file
//  Created on:      30-Jul-2005 14:40:00
//  Original author: LN
///////////////////////////////////////////////////////////

#include "BaseMath.h"
#include "Entity.h"
#include "CParseLevel.h"

namespace gen
{

malaz::ERenderMethod Method( std::string name )
{
	if( name == "PixelLighting" )
	{
		return malaz::PixelLighting;
	}
	else if( name == "PixelLightingSpec" )
	{
		return malaz::PixelLightingSpec;
	}
	else if( name == "NormalMapping" )
	{
		return malaz::NormalMapping;
	}
	else if( name == "VertexLighting" )
	{
		return malaz::VertexLighting;
	}
	else if( name == "BrightenedTexture" )
	{
		return malaz::BrightenedTexture;
	}
	else if( name == "Black" )
	{
		return malaz::Black;
	}
	else
	{
		return malaz::Texture;
	}
}

malaz::ePowerUp PowerUp( std::string name )
{
	if( name == "MachineGun" )
	{
		return malaz::MachineGun;
	}
	else if( name == "Mortar" )
	{
		return malaz::Mortar;
	}
	else if( name == "HomingMissile" )
	{
		return malaz::HomingMissile;
	}
	else if( name == "SpeedBoost" )
	{
		return malaz::SpeedBoost;
	}
	else if( name == "Heal" )
	{
		return malaz::Heal;
	}
	else if( name == "Invincible" )
	{
		return malaz::Invincible;
	}
	else
	{
		return malaz::None;
	}
}

/*---------------------------------------------------------------------------------------------
	Constructors / Destructors
---------------------------------------------------------------------------------------------*/

// Constructor initialises state variables
CParseLevel::CParseLevel( malaz::CEntityManager* entityManager, malaz::CTrack* Track, malaz::CLightManager* lightManager  )
{
	// Take copy of entity manager for creation
	m_EntityManager = entityManager;

	// Hold pointer to the track for the level
	m_Track = Track;

	// File state
	m_CurrentSection = None;

	m_LightManager = lightManager;

	// Template state
	m_TemplateType = "";
	m_TemplateName = "";
	m_TemplateMesh = "";

	// Entity state
	m_EntityType = "";
	m_EntityName = "";
	m_Pos = CVector3::kOrigin;
	m_Rot = CVector3::kOrigin;
	m_Scale = CVector3(1.0f, 1.0f, 1.0f);
}


/*---------------------------------------------------------------------------------------------
	Callback Functions
---------------------------------------------------------------------------------------------*/

// Callback function called when the parser meets the start of a new element (the opening tag).
// The element name is passed as a string. The attributes are passed as a list of (C-style)
// string pairs: attribute name, attribute value. The last attribute is marked with a null name
void CParseLevel::StartElt( const string& eltName, SAttribute* attrs )
{
	// Open major file sections
	if (eltName == "Templates")
	{
		m_CurrentSection = Templates;
	}
	else if (eltName == "Entities")
	{
		m_CurrentSection = Entities;
	}
	else if (eltName == "Lighting")
	{
		m_CurrentSection = Lighting;
	}

	// Different parsing depending on section currently being read
	switch (m_CurrentSection)
	{
		case Templates:
			TemplatesStartElt( eltName, attrs ); // Parse template start elements
			break;
		case Entities:
			EntitiesStartElt( eltName, attrs ); // Parse entity start elements
			break;
		case Lighting:
			LightStartElt( eltName, attrs ); // Parse entity start elements
			break;
	}
}

// Callback function called when the parser meets the end of an element (the closing tag). The
// element name is passed as a string
void CParseLevel::EndElt( const string& eltName )
{
	// Close major file sections
	if ( eltName == "Templates" || eltName == "Entities" || eltName == "Lighting" )
	{
		m_CurrentSection = None;
	}

	// Different parsing depending on section currently being read
	switch (m_CurrentSection)
	{
		case Templates:
			TemplatesEndElt( eltName ); // Parse template end elements
			break;
		case Entities:
			EntitiesEndElt( eltName ); // Parse entity end elements
			break;
		case Lighting:
			LightEndElt( eltName ); // Parse entity end elements
			break;
	}
}


/*---------------------------------------------------------------------------------------------
	Section Parsing
---------------------------------------------------------------------------------------------*/

// Called when the parser meets the start of an element (opening tag) in the templates section
void CParseLevel::TemplatesStartElt( const string& eltName, SAttribute* attrs )
{
	// Started reading a new entity template - get type, name and mesh
	if (eltName == "EntityTemplate")
	{
		// Get attributes held in the tag
		m_TemplateName = GetAttribute( attrs, "Name" );
		m_TemplateType = GetAttribute( attrs, "Type" );
		m_TemplateMesh = GetAttribute( attrs, "Mesh" );
		m_Folder = GetAttribute( attrs, "Folder" );
		m_RenderMethod = Method( GetAttribute( attrs, "RenderMethod" ) );
		m_MaxSpeed = GetAttributeFloat( attrs, "MaxSpeed" );
		m_Acceleration = GetAttributeFloat( attrs, "Acceleration" );
		m_TurnSpeed = GetAttributeFloat( attrs, "TurnSpeed" );
		m_TurretTurnSpeed = GetAttributeFloat( attrs, "TurretTurnSpeed" );
		m_MaxHP = GetAttributeInt( attrs, "MaxHP" );
		m_Texture = GetAttribute( attrs, "Texture" );
		m_PowerUpType = PowerUp( GetAttribute( attrs, "PowerUpType" ) );
		m_numBullets = GetAttributeInt( attrs, "Bullets" );
	}
}

// Called when the parser meets the end of an element (closing tag) in the templates section
void CParseLevel::TemplatesEndElt( const string& eltName )
{
	// Finished reading an entity template - create it using parsed data
	if (eltName == "EntityTemplate")
	{
		CreateEntityTemplate();
	}
}

// Called when the parser meets the start of an element (opening tag) in the entities section
void CParseLevel::EntitiesStartElt( const string& eltName, SAttribute* attrs )
{
	// Started reading a new entity - get type and name
	if (eltName == "Entity") 
	{
		m_EntityType = GetAttribute( attrs, "Type" );
		m_EntityName = GetAttribute( attrs, "Name" );

		m_Lane = GetAttributeInt( attrs, "Lane" );

		// Set default positions
		m_Pos = CVector3::kOrigin;
		m_Rot = CVector3::kOrigin;
		m_Scale = CVector3(1.0f, 1.0f, 1.0f);
	}
	// Started reading an entity position - get X,Y,Z
	if (eltName == "Position")
	{
		m_Pos.x = GetAttributeFloat( attrs, "X" );
		m_Pos.y = GetAttributeFloat( attrs, "Y" );
		m_Pos.z = GetAttributeFloat( attrs, "Z" );
	}
	// Started reading an entity rotation - get X,Y,Z
	else if (eltName == "Rotation")
	{
		m_Rot.x = ToRadians(GetAttributeFloat( attrs, "X" ));
		m_Rot.y = ToRadians(GetAttributeFloat( attrs, "Y" ));
		m_Rot.z = ToRadians(GetAttributeFloat( attrs, "Z" ));
	}
	// Started reading an entity scale - get X,Y,Z
	else if (eltName == "Scale")
	{
		m_Scale.x = GetAttributeFloat( attrs, "X" );
		m_Scale.y = GetAttributeFloat( attrs, "Y" );
		m_Scale.z = GetAttributeFloat( attrs, "Z" );
	}

	// Start reading a track pieces
	else if (eltName == "Bounds")
	{
		m_Bounds.x = GetAttributeFloat( attrs, "Width" );
		m_Bounds.y = GetAttributeFloat( attrs, "Length" );
	}

	// Randomising an entity position - get X,Y,Z amounts and randomise
	else if (eltName == "Randomise")
	{
		float randomX = GetAttributeFloat( attrs, "X" ) * 0.5f;
		float randomY = GetAttributeFloat( attrs, "Y" ) * 0.5f;
		float randomZ = GetAttributeFloat( attrs, "Z" ) * 0.5f;
		m_Pos.x += Random( -randomX, randomX );
		m_Pos.y += Random( -randomY, randomY );
		m_Pos.z += Random( -randomZ, randomZ );
	}
	else if (eltName == "Waypoints")
	{
		m_Lanes = new vector<CVector3>[malaz::LANES];
	}

	if( eltName == "Waypoint" )
	{
		CVector3 pos;
		pos.x = GetAttributeFloat( attrs, "X" );
		pos.y = GetAttributeFloat( attrs, "Y" );
		pos.z = GetAttributeFloat( attrs, "Z" );

		int Lane = GetAttributeInt( attrs, "Lane" );		
		m_Lanes[Lane].push_back( pos );
		int i=0;
	}
}

// Called when the parser meets the end of an element (closing tag) in the entities section
void CParseLevel::EntitiesEndElt( const string& eltName )
{
	// Finished reading entity - create it using parsed data
	if (eltName == "Entity")
	{
		CreateEntity();
	}
}

void CParseLevel::LightStartElt(const std::string &eltName, gen::CParseXML::SAttribute *attrs)
{
	if (eltName == "AmbientLight")
	{
		float r,g,b;
		r = GetAttributeFloat( attrs, "R" );
		g = GetAttributeFloat( attrs, "G" );
		b = GetAttributeFloat( attrs, "B" );

		m_LightManager->SetAmbientLight( r, g, b );
	}

	if (eltName == "Light")
	{
		m_Brightness = GetAttributeFloat( attrs, "Brightness" );
		m_TemplateName = GetAttribute( attrs, "Template" );
	}

	if (eltName == "Colour")
	{
		m_Colour.r = GetAttributeFloat( attrs, "R" );
		m_Colour.g = GetAttributeFloat( attrs, "G" );
		m_Colour.b = GetAttributeFloat( attrs, "B" );
	}

	if (eltName == "Position")
	{
		m_Pos.x = GetAttributeFloat( attrs, "X" );
		m_Pos.y = GetAttributeFloat( attrs, "Y" );
		m_Pos.z = GetAttributeFloat( attrs, "Z" );
	}
}

void CParseLevel::LightEndElt(const std::string &eltName)
{
	if (eltName == "Light")
	{
		CreateLight();
	}
}

void CParseLevel::CreateLight()
{
	m_LightManager->CreateLight( m_Pos, m_Colour, m_Brightness );
	m_EntityManager->CreateEntity( m_TemplateName, "Light", m_Pos, CVector3( 0.0f, 0.0f, 0.0f ), CVector3( 0.2f, 0.2f, 0.2f ) );
}

/*---------------------------------------------------------------------------------------------
	Entity Template and Instance Creation
---------------------------------------------------------------------------------------------*/

// Create an entity template using data collected from parsed XML elements
void CParseLevel::CreateEntityTemplate()
{
	// Initialise the template depending on its type

	// Tank Template
	if( m_TemplateType == "Tank" || m_TemplateType == "Player Tank" || m_TemplateType == "AI Tank" )
	{
		m_EntityManager->CreateTankTemplate( m_TemplateName, m_TemplateType, m_Folder, m_TemplateMesh, m_RenderMethod, m_Texture, m_MaxSpeed,
											 m_Acceleration, m_TurnSpeed, m_Weight, m_MaxHP );
	}
	else if( m_TemplateType == "Power Up" )
	{
		m_EntityManager->CreatePowerUpTemplate( m_TemplateName, m_TemplateType, m_Folder, m_TemplateMesh, m_RenderMethod, m_PowerUpType, m_numBullets, m_Texture );
	}
	else
	{
		m_EntityManager->CreateEntityTemplate( m_TemplateName, m_TemplateType, m_Folder, m_TemplateMesh, m_RenderMethod, m_Texture );
	}
}

// Create an entity using data collected from parsed XML elements
void CParseLevel::CreateEntity()
{
	// Get the type of template that this entity uses
	string templateType = m_EntityManager->GetTemplate( m_EntityType )->Type();

	// Create a new entity of this template type
	if (templateType == "Player Tank")
	{
		m_EntityManager->CreatePlayerTank( m_EntityType, m_EntityName, m_Track, m_Pos, m_Rot, m_Scale );
	}
	else if (templateType == "Power Up")
	{
		m_EntityManager->CreatePowerUpEntity( m_EntityType, m_EntityName, m_Pos, m_Rot, m_Scale );
	}
	else if (templateType == "AI Tank")
	{
		m_EntityManager->CreateAITank( m_EntityType, m_EntityName, m_Track, m_Lane, m_Pos, m_Rot, m_Scale );
	}
	else if (templateType == "Track")
	{
		// If the entity is a piece of the track add it to the track object
		m_Track->Add( m_EntityManager->CreateTrackEntity( m_EntityType, m_EntityName, m_Bounds, m_Lanes, m_Pos, m_Rot, m_Scale ) );
	}
	else
	{
		m_EntityManager->CreateEntity( m_EntityType, m_EntityName, m_Pos, m_Rot, m_Scale );
	}
}


} // namespace gen
