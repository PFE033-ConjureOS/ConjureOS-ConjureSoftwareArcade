from MetadataProperties import MetadataProperties


class MetaDataObject:
    collection = "Members Games"
    metadata_id = "00000000-0000-0000-0000-000000000000"
    game = "Game Title test"
    version = "0.1.1"
    description = "test_description"
    leaderboard = "False"
    thumbnailPath = "medias\\thumbnail.png"
    imagePath = "medias\\gameplayImage.png"

    def get(self, metadata_property: MetadataProperties):
        match metadata_property:
            case MetadataProperties.COLLECTION:
                return self.collection
            case MetadataProperties.METADATA_ID:
                return self.metadata_id
            case MetadataProperties.GAME:
                return self.game
            case MetadataProperties.VERSION:
                return self.version
            case MetadataProperties.DESCRIPTION:
                return self.description
            case MetadataProperties.LEADERBOARD:
                return self.leaderboard
            case MetadataProperties.THUMBNAIL_PATH:
                return self.thumbnailPath
            case MetadataProperties.IMAGE_PATH:
                return self.imagePath
